#include "ModuleAppControl.h"

#include "AppController.h"

#include <QFile>



namespace SwissalpS { namespace QtSssSapp {



ModuleAppControl::ModuleAppControl(ModuleConf *pConf, QObject *pParent) :
	ModuleBase(pConf, pParent),
	pAC(nullptr),
	pAS(nullptr),
	pWebSocketServer(nullptr),
	pWWWSserver(nullptr) {

} // construct


ModuleAppControl::~ModuleAppControl() {

} // dealloc


bool ModuleAppControl::executeBusCommand(const QString &sCommand,
								 const QJsonObject &ojoMessage) {


	// an empty sender uid is valid -> respond to all modules
	QString sSender = ojoMessage.value("sender").toString("");

	// missing respond command means we can't respond, which is not required at times
	QString sRespondCommand = ojoMessage.value("respondCommand").toString("NULL");
	bool bRespond = (0 != sRespondCommand.compare("NULL"));

	// prepare response object
	QJsonObject ojoResponse;

	if (0 == sCommand.compare("quit")) {

		this->pAC->quit();

		return true;

	} else {

		//this->onDebugMessage(tr("KO:Command not coded: " + sCommand));

		// give base a chance to execute
		return ModuleBase::executeBusCommand(sCommand, ojoMessage);

	} // switch command

	if (!bRespond) return true;

	ojoResponse.insert("sender", this->pMC->moduleUID());
	ojoResponse.insert("command", sRespondCommand);
	ojoResponse.insert("context", ojoMessage.value("context").toString(""));

	Q_EMIT this->busMessage(QStringList() << sSender, ojoResponse);

	return true;

} // executeCommand


QStringList ModuleAppControl::exposedBusCommands() const {

	static QStringList asList = ModuleBase::exposedBusCommands();

	asList.append("quit");

	return asList;

} // exposedCommands


void ModuleAppControl::die() {

	this->stop();

	ModuleBase::die();

	delete this->pWebSocketServer;
	this->pWebSocketServer = nullptr;

	delete this->pWWWSserver;
	this->pWWWSserver = nullptr;

} // die


void ModuleAppControl::init() {

	ModuleBase::init();

	this->pAC = AppController::pAppController();

	this->pAS = AppSettings::pAppSettings();

	this->initSSL();

	this->initAuthentication();

	this->initWebSocketServer();

	this->initWWWserver();

} // init


void ModuleAppControl::initAuthentication() {

	this->onDebugMessage("initAuthentication");

	ModuleBase::initAuthentication();

} // initAuthentication


void ModuleAppControl::initSSL() {

	this->onDebugMessage("initSSL");

	ModuleBase::initSSL();

} // initSSL


void ModuleAppControl::initWebSocketServer() {

	// don't even think about initiating without certificate and key
	if (this->oCertificate.isNull() || this->oPrivateKey.isNull()) return;

	// only run once
	if (nullptr != this->pWebSocketServer) return;

	this->pWebSocketServer = new WSSserver(this);

	connect(this->pWebSocketServer, SIGNAL(debugMessage(QString)),
			this, SLOT(onDebugMessage(QString)));

	connect(this->pWebSocketServer, SIGNAL(gotJSON(QString,QJsonObject,WSSsession*)),
			this, SLOT(onGotJSON(QString,QJsonObject,WSSsession*)));

	this->pWebSocketServer->setListBW(this->asListBW);
	this->pWebSocketServer->setListIsBlack(this->pMC->isListBlack());
	this->pWebSocketServer->setMaxConnections(this->pMC->maxConnections());

	QHostAddress oHost = ModuleBase::stringToHostAddress(this->pMC->localIP());
	this->pWebSocketServer->init(oHost, this->pMC->localPort1(),
								 this->oCertificate, this->oPrivateKey);

} // initWebSocketServer


void ModuleAppControl::initWWWserver() {

	// only init if there is already a socket server ready
	if (nullptr == this->pWebSocketServer) return;

	// only run once
	if (nullptr != this->pWWWSserver) return;

	this->pWWWSserver = new WWWSserver(this);

	connect(this->pWWWSserver, SIGNAL(debugMessage(QString)),
			this, SLOT(onDebugMessage(QString)));

	connect(this->pWWWSserver, SIGNAL(gotRequest(WWWSrequest*)),
			this, SLOT(onWWWSrequest(WWWSrequest*)));

	this->pWWWSserver->setServerString("SwissalpS ModuleAppControl");
	this->pWWWSserver->setListBW(this->asListBW);
	this->pWWWSserver->setListIsBlack(this->pMC->isListBlack());
	this->pWWWSserver->setMaxConnections(this->pMC->maxConnections());

	QHostAddress oHost = ModuleBase::stringToHostAddress(this->pMC->localIP());

	this->pWWWSserver->init(oHost, this->pMC->localPort(),
							this->oCertificate, this->oPrivateKey);

} // initWWWserver


void ModuleAppControl::handleLogin(QJsonObject oJarguments, WSSsession *pSession) {

	this->onDebugMessage("handleLogin");

	bool bUserOK = 0 == this->pMC->user().compare(oJarguments.value("user").toString());
	bool bPassOK = 0 == this->pMC->password().compare(oJarguments.value("password").toString());
	bool bNonceOK = !oJarguments.value("nonce").toString().isEmpty();

	bool bAuthenticated = bUserOK && bPassOK && bNonceOK;

	QJsonObject oJresponse;
	oJresponse.insert("command", "authenticated");
	oJresponse.insert("result", bAuthenticated);

	pSession->socket()->sendTextMessage(QString(QJsonDocument(oJresponse).toJson()));

} // handleLogin


void ModuleAppControl::onGotJSON(const QString &sCommand,
								 QJsonObject oJarguments, WSSsession *pSession) {

	this->onDebugMessage("onGotJSON " + sCommand);

	if (pSession->socket()->sslConfiguration().isNull()) {

		this->onDebugMessage("KO:Unencrypted WebSocket request snuck in");
		return;

	} // if not encrypted

	if (0 == sCommand.compare("login")) {

		this->handleLogin(oJarguments, pSession);
		return;

	} // switch known commands

	this->onDebugMessage("KO:Unknown command from WebSocket: " + sCommand);

} // onGotJSON


void ModuleAppControl::onWWWSrequest(WWWSrequest *pRequest) {

	this->onDebugMessage("onWWWSrequest");

	if (!pRequest->socket()->isEncrypted()) {

		this->onDebugMessage("KO:Unencrypted request. Dropping");

		this->pWWWSserver->respond404(pRequest);

		return;

	} // if unencrypted snuck in

	bool b404 = false;
	if (!pRequest->isGet()) b404 = true;
	else if (0 != pRequest->path().compare("/")) b404 = true;

	if (b404) {

		this->pWWWSserver->respond404(pRequest);

		return;

	} //

	QFile oFile(":/www/moduleAppControl.html");
	if (!oFile.open(QIODevice::ReadOnly)) {

		this->pWWWSserver->respond404(pRequest);

		return;

	} // if failed to open template

	QString sBody = QString::fromUtf8(oFile.readAll());
	oFile.close();

	QString sURI = "wss://" + this->pWebSocketServer->host().toString()
				   + ":" + QString::number(this->pWebSocketServer->port());

	static QString sPool = "abcdefghijklmnopqrstuvwxyz0123456789._ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	QString sNonce;
	while (64 >= sNonce.length()) sNonce += sPool.at(qrand() % sPool.length());

	sBody.replace("##wssURI_SwissalpS_REPLACER_TAG##", sURI);
	sBody.replace("##nonce_SwissalpS_REPLACER_TAG##", sNonce);

	this->pWWWSserver->respond(pRequest, sBody);

} // onWWWSrequest


void ModuleAppControl::start() {

	// ignore if not active
	if (!this->isActive()) return;

	ModuleBase::start();

	// in case init had not been called
	if (nullptr == this->pWebSocketServer) this->init();

	this->startWebSocketServer();

	this->startWWWserver();

} // start


void ModuleAppControl::startWebSocketServer() {

	if (nullptr == this->pWebSocketServer) return;

	this->pWebSocketServer->start();

} // startWebSocketServer


void ModuleAppControl::startWWWserver() {

	if (nullptr == this->pWWWSserver) return;
	if (nullptr == this->pWebSocketServer) return;

	if (!this->pWebSocketServer->isListening()) {

		this->onDebugMessage(tr("KO:Web-Socket-Server is not running. Will not start WWW-Server."));

		return;

	} // if no WSS

	this->pWWWSserver->start();

	this->onDebugMessage(tr("OK:You can access AppControl-module from your browser\n  URL: https://")
						 + this->pWWWSserver->host().toString()
						 + ":" + QString::number(this->pWWWSserver->port())
						 + "/\n  Username: " + this->pMC->user()
						 + "\n  Password: " + this->pMC->password());

} // startWWWserver


void ModuleAppControl::stop() {

	this->stopWWWserver();

	this->stopWebSocketServer();

	ModuleBase::stop();

} // stop


void ModuleAppControl::stopWebSocketServer() {

	this->pWebSocketServer->stop();

} // stopWebSocketServer


void ModuleAppControl::stopWWWserver() {

	this->pWWWSserver->stop();

} // stopWWWserver



}	} // namespace SwissalpS::QtSssSapp
