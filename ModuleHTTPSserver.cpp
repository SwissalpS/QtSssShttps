#include "ModuleHTTPSserver.h"

#include <QDir>
#include <QFileInfo>
#include <QHostInfo>
#include <QRegularExpressionMatch>

#include "MHTTPSShandlerDrop.h"
#include "MHTTPSShandlerFileServer.h"
#include "MHTTPSShandlerRewritePath.h"



namespace SwissalpS { namespace QtSssSapp {



ModuleHTTPSserver::ModuleHTTPSserver(ModuleConf *pConf, QObject *pParent) :
	ModuleBase(pConf, pParent),
	pWWWSserver(nullptr) {

} // ModuleHTTPSserver::construct


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
	this->pWWWSserver->setBasePaths(this->pMC->basePaths());

	QHostAddress oHost = ModuleBase::stringToHostAddress(this->pMC->localIP());

	this->pWWWSserver->init(oHost, this->pMC->localPort(),
							this->oCertificate, this->oPrivateKey);

} // init


void ModuleHTTPSserver::initHandlers() {

	QJsonArray ojaHandlers = this->pMC->toJSONobject().value("aHandlers").toArray();

	int iTotalIn = ojaHandlers.count();

	this->onDebugMessage(tr("OK:initiating %1 handler%2...")
						 .arg(iTotalIn)
						 .arg((1 == iTotalIn) ? "" : "s"));

	for (int i = 0; i < ojaHandlers.count(); ++i) {

		this->onAppendHandler(ojaHandlers.at(i).toObject());

	} // loop

	int iTotal = this->apHandlers.length();
	this->onDebugMessage(QString("OK:initiated %1/%3 handler%2")
						 .arg(iTotal)
						 .arg((1 == iTotalIn) ? "" : "s")
						 .arg(iTotalIn));

} // initHandlers


void ModuleHTTPSserver::onAppendHandler(QJsonObject ojoHandler) {

	bool bActive = ojoHandler.value(ModuleConf::sTagActive).toBool(false);
	if (!bActive) return;

	QString sClass = ojoHandler.value(ModuleConf::sTagClass).toString();
	MHTTPSShandlerBase *pHandler;

	if (0 == sClass.compare("MHTTPSShandlerBase")) {

		// ignore for now
		return;

	} else if (0 == sClass.compare("MHTTPSShandlerDrop")) {

		MHTTPSShandlerDrop *pHDrop = new MHTTPSShandlerDrop(
											   ojoHandler, this);

		pHandler = pHDrop;

	} else if (0 == sClass.compare("MHTTPSShandlerFileServer")) {

		MHTTPSShandlerFileServer *pHDrop = new MHTTPSShandlerFileServer(
											   ojoHandler, this);

		pHandler = pHDrop;

	} else if (0 == sClass.compare("MHTTPSShandlerRedirect")) {

		MHTTPSShandlerRedirect *pHRed = new MHTTPSShandlerRedirect(
											   ojoHandler, this);

		pHandler = pHRed;

	} else if (0 == sClass.compare("MHTTPSShandlerRewritePath")) {

		MHTTPSShandlerRewritePath *pHRew = new MHTTPSShandlerRewritePath(
											  ojoHandler, this);

		pHandler = pHRew;

	} else {

		this->onDebugMessage("KO:Invalid Handler class name for "
							 "ModuleHTTPSserver: " + sClass);

		return;

	} // switch class

	this->onAppendHandler(pHandler);

} // onAppendHandler(QJsonObject)


void ModuleHTTPSserver::onAppendHandler(MHTTPSShandlerBase *pHandler) {

	connect(pHandler, SIGNAL(debugMessage(QString)),
			this, SLOT(onDebugMessage(QString)));

	connect(pHandler, SIGNAL(ignore(WWWSrequest*)),
			this, SLOT(onIgnore(WWWSrequest*)));

	connect(pHandler, SIGNAL(nextHandler(WWWSrequest*)),
			this, SLOT(onNextHandler(WWWSrequest*)));

	connect(pHandler, SIGNAL(respond(WWWSrequest*,QString,quint16)),
			this, SLOT(onRespond(WWWSrequest*,QString,quint16)));

	connect(pHandler, SIGNAL(respond(WWWSresponse*)),
			this, SLOT(onRespond(WWWSresponse*)));

	connect(pHandler, SIGNAL(respond200(WWWSrequest*,QString,QString)),
			this, SLOT(onRespond200(WWWSrequest*,QString,QString)));

	connect(pHandler, SIGNAL(respond301(WWWSrequest*,QString)),
			this, SLOT(onRespond301(WWWSrequest*,QString)));

	connect(pHandler, SIGNAL(respond404(WWWSrequest*)),
			this, SLOT(onRespond404(WWWSrequest*)));

	connect(pHandler, SIGNAL(respondDirectoryList(WWWSrequest*,QString)),
			this, SLOT(onRespondDirectoryList(WWWSrequest*,QString)));

	connect(pHandler, SIGNAL(respondFile(WWWSrequest*,QString)),
			this, SLOT(onRespondFile(WWWSrequest*,QString)));

	connect(pHandler, SIGNAL(respondJSON(WWWSrequest*,QString)),
			this, SLOT(onRespondJSON(WWWSrequest*,QString)));

	QString sClass = pHandler->className();
	this->onDebugMessage("OK:Initializing handler of class: " + sClass);
	pHandler->init();

	if (pHandler->isActive()) {

		this->apHandlers.append(pHandler);

		this->onDebugMessage(tr("OK:Initialized and added handler of class: ")
							 + sClass);

	} else {

		this->onDebugMessage("KO:Due to init error, dropped handler of class: "
							 + sClass);
		disconnect(pHandler);
		delete pHandler;

	} // if still active or not

} // onAppendHandler(MHTTPSShandlerBase)


void ModuleHTTPSserver::onIgnore(WWWSrequest *pRequest) {

	this->pWWWSserver->ignore(pRequest);

} // onIgnore


void ModuleHTTPSserver::onNextHandler(WWWSrequest *pRequest) {

	this->onDebugMessage("onNextHandler");

	bool bPolledAll = true;

	const QString sMethod = pRequest->method();
	QString sMethodNot = "^" + sMethod;
	MHTTPSShandlerBase *pHandler;
	while (this->apHandlers.length() > pRequest->ubHandlerIndex) {

		pHandler = this->apHandlers.at(pRequest->ubHandlerIndex);

		pRequest->ubHandlerIndex++;

		// skip if handler explicitly does not want this method
		if (pHandler->methods().contains(sMethodNot)) continue;
		// skip if request method is not in list of this handlers methods
		if (!pHandler->methods().contains(sMethod)) continue;
		// skip if this handler is a rewriter but rewrites have completed
		if (pHandler->isRewriteHandlerClass() && pRequest->bRewritesDone) continue;

		bPolledAll = false;

		pHandler->handle(pRequest);
		return;

	} // loop until find one that is active

	if (bPolledAll) this->pWWWSserver->respond404(pRequest);

} // onNextHandler


void ModuleHTTPSserver::onRequest(WWWSrequest *pRequest) {

	this->onDebugMessage("onRequest " + pRequest->method() + " "
						 + pRequest->targetQ().path());

	if (!pRequest->socket()->isEncrypted()) {

		this->onDebugMessage("KO:Unencrypted request. Dropping");

		this->pWWWSserver->respond404(pRequest);

		return;

	} // if unencrypted snuck in

	this->onNextHandler(pRequest);

} // onRequest


void ModuleHTTPSserver::onRespond(WWWSresponse *pResponse) {

	this->onDebugMessage("onRespond pResponse");

	this->pWWWSserver->respond(pResponse);

} // onRespond pResponse


void ModuleHTTPSserver::onRespond(WWWSrequest *pRequest, const QString &sBody,
								  const quint16 uiCode) {

	this->onDebugMessage("onRespond pRequest sBody uiCode");

	this->pWWWSserver->respond(pRequest, sBody, uiCode);

} // onRespond pRequest sBody uiCode


void ModuleHTTPSserver::onRespond200(WWWSrequest *pRequest,
									 const QString &sBody,
									 const QString &sContentHeaderValue) {

	this->onDebugMessage("onRespond200");

	this->pWWWSserver->respond200(pRequest, sBody, sContentHeaderValue);

} // onRespond200


void ModuleHTTPSserver::onRespond404(WWWSrequest *pRequest) {

	this->onDebugMessage("onRespond404");

	this->pWWWSserver->respond404(pRequest);

} // onRespond404


void ModuleHTTPSserver::onRespond301(WWWSrequest *pRequest, const QString &sURL) {

	this->onDebugMessage("onRespond301");

	this->pWWWSserver->respond301(pRequest, sURL);

} // onRespond301


void ModuleHTTPSserver::onRespondDirectoryList(WWWSrequest *pRequest,
											   const QString &sPath) {

	this->onDebugMessage("onRespondDirectoryList");

	this->pWWWSserver->respondDirectoryList(pRequest, sPath);

} // onRespondDirectoryList


void ModuleHTTPSserver::onRespondFile(WWWSrequest *pRequest, const QString &sPathFile) {

	this->onDebugMessage("onRespondFile");

	this->pWWWSserver->respondFile(pRequest, sPathFile);

} // onRespondFile


void ModuleHTTPSserver::onRespondJSON(WWWSrequest *pRequest, const QString &sJSON) {

	this->onDebugMessage("onRespondJSON");

	this->pWWWSserver->respondJSON(pRequest, sJSON);

} // onRespondJSON


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
