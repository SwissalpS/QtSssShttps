#include "ModuleHTTPSserver.h"

#include <QHostInfo>



namespace SwissalpS { namespace QtSssSapp {



ModuleHTTPSserver::ModuleHTTPSserver(ModuleConf *pConf, QObject *pParent) :
	ModuleBase(pConf, pParent) {

} // construct


ModuleHTTPSserver::~ModuleHTTPSserver() {

} // dealloc


void ModuleHTTPSserver::die() {

	this->stop();

	ModuleBase::die();

	delete this->pWWWSserver;
	this->pWWWSserver = nullptr;

} // die


void ModuleHTTPSserver::init() {

	// only run once
	if (nullptr != this->pWWWSserver) return;

	ModuleBase::init();

	this->apHandlers.clear();

	this->initSSL();

	this->initAuthentication();

	this->initHandlers();

	this->pWWWSserver = new WWWSserver(this);

	connect(this->pWWWSserver, SIGNAL(debugMessage(QString)),
			this, SLOT(onDebugMessage(QString)));

	connect(this->pWWWSserver, SIGNAL(gotRequest(WWWSrequest*)),
			this, SLOT(onRequest(WWWSrequest*)));

	this->pWWWSserver->setServerString("SwissalpS ModuleHTTPSserver");
	this->pWWWSserver->setListBW(this->asListBW);
	this->pWWWSserver->setListIsBlack(this->pMC->isListBlack());
	this->pWWWSserver->setMaxConnections(this->pMC->maxConnections());

	QHostAddress oHost = ModuleBase::stringToHostAddress(this->pMC->localIP());

	this->pWWWSserver->init(oHost, this->pMC->localPort(),
							this->oCertificate, this->oPrivateKey);

} // init


void ModuleHTTPSserver::initHandlers() {

	QJsonArray ojaHandlers = this->pMC->toJSONobject().value("aHandlers").toArray();

	for (int i = 0; i < ojaHandlers.count(); ++i) {

		this->onAppendHandler(ojaHandlers.at(i).toObject());

	} // loop

} // initHandlers


void ModuleHTTPSserver::onAppendHandler(QJsonObject ojoHandler) {

	QString sClass = ojoHandler.value(ModuleConf::sTagClass).toString();
	MHTTPSShandlerBase *pHandler;
	if (0 == sClass.compare("MHTTPSShandlerBase")) {

		// ignore for now
		return;

	} else if (0 == sClass.compare("MHTTPSShandlerRedirect")) {

	} else {

		this->onDebugMessage("KO:Invalid class name for Handler for "
							 "ModuleHTTPSserver: " + sClass);
		return;

	} // switch class

	this->onAppendHandler(pHandler);

} // onAppendHandler(QJsonObject)


void ModuleHTTPSserver::onAppendHandler(MHTTPSShandlerBase *pHandler) {

	this->apHandlers.append(pHandler);

	connect(pHandler, SIGNAL(debugMessage(QString)),
			this, SLOT(onDebugMessage(QString)));

	connect(pHandler, SIGNAL(nextHandler(WWWSrequest*)),
			this, SLOT(onNextHandler(WWWSrequest*)));

} // onAppendHandler(MHTTPSShandlerBase)


void ModuleHTTPSserver::onNextHandler(WWWSrequest *pRequest) {

	this->onDebugMessage("onNextHandler");

	quint8 ubIndex = pRequest->ubHandlerIndex;
	if (this->apHandlers.length() > ubIndex) {

		pRequest->ubHandlerIndex++;

		this->apHandlers.at(ubIndex)->handle(pRequest);

	} else this->pWWWSserver->respond404(pRequest);

} // onNextHandler


void ModuleHTTPSserver::onRequest(WWWSrequest *pRequest) {

	this->onDebugMessage("onRequest");

	if (!pRequest->socket()->isEncrypted()) {

		this->onDebugMessage("KO:Unencrypted request. Dropping");

		this->pWWWSserver->respond404(pRequest);

		return;

	} // if unencrypted snuck in

	this->onNextHandler(pRequest);

} // onRequest


void ModuleHTTPSserver::start() {

	// ignore if not active
	if (!this->isActive()) return;

	ModuleBase::start();

	// in case init had not been called
	if (nullptr == this->pWWWSserver) this->init();

	this->pWWWSserver->start();

//	this->onDebugMessage(tr("OK:You can access HTTPS-server-module from your browser\n  URL: https://")
//						 + this->pWWWSserver->host().toString()
//						 + ":" + QString::number(this->pWWWSserver->port())
//						 + "/\n  Username: " + this->pMC->user()
//						 + "\n  Password: " + this->pMC->password());


} // start


void ModuleHTTPSserver::stop() {

	this->pWWWSserver->stop();

	ModuleBase::stop();

} // stop


QHostAddress ModuleHTTPSserver::stringToHostAddress(const QString &sHostOrIP) {

	QHostAddress oHost(sHostOrIP);
	if (QAbstractSocket::UnknownNetworkLayerProtocol == oHost.protocol()) {

		// is not an IP, probably hostname. Let's look up IP
		QHostInfo oHostInfo = QHostInfo::fromName(sHostOrIP);
		QList<QHostAddress> aoIPs = oHostInfo.addresses();
		oHost = QHostAddress();
		if (aoIPs.isEmpty()) {

			//this->onDebugMessage("KO:Can not resolve IP/Host: " + sHostOrIP);
			return oHost;

		} // if could not resolve IP

		for (int i = 0; i < aoIPs.length(); ++i) {

			oHost = aoIPs.at(i);
			if (QAbstractSocket::IPv4Protocol == oHost.protocol()) break;

		} // loop

	} // if not an IP in config

	return oHost;

} // stringToHostAddress



}	} // namespace SwissalpS::QtSssSapp
