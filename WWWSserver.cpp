#include "WWWSserver.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMimeDatabase>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>



namespace SwissalpS { namespace QtSssSapp {



WWWSserver::WWWSserver(QObject *pParent) :
	QTcpServer(pParent),
	bListIsBlack(false),
	pTCPserver(nullptr),
	sServerString("SwissalpS WWWSserver"),
	uSessionCounter(0u),
	uiMaxConnections(25u),
	uiMaxRequestSize(2000000u),
	uiPort(0u) {

	this->asListBasePaths.clear();
	this->asListBW.clear();
	this->apSessions.clear();

} // construct


WWWSserver::~WWWSserver() {

	this->pTCPserver->close();
	qDeleteAll(this->apSessions);

} // dealloc


QString WWWSserver::currentRFC7231date() {

	return QDateTime::currentDateTimeUtc().toString("ddd, d MMM yyyy hh:mm:ss") + " GMT";

} // currentRFC7231date


void WWWSserver::ignore(WWWSrequest *pRequest) {

	pRequest->socket()->abort();

} // ignore


void WWWSserver::incomingConnection(qintptr ipSocketDescriptor) {

	QSslSocket *pSSLsocket;
	WWWSsession *pSession;

	// prepare SSL socket
	pSSLsocket = new QSslSocket();
	pSSLsocket->setLocalCertificate(this->oCertificate);
	pSSLsocket->setPrivateKey(this->oPrivateKey);
	pSSLsocket->setProtocol(QSsl::TlsV1SslV3);
	pSSLsocket->setPeerVerifyMode(QSslSocket::VerifyNone); // VerifyNone, QueryPeer, VerifyPeer, AutoVerifyPeer

	if (!pSSLsocket->setSocketDescriptor(ipSocketDescriptor)) {

		this->onDebugMessage(tr("KO:failed to hand over socket"));
		delete pSSLsocket;
		return;

	} // if could not hand over socket discriptor

	bool bDestroy = false;
	if (this->isBlockedIP(pSSLsocket->peerAddress().toString())) bDestroy = true;
	if (this->uiMaxConnections <= this->apSessions.length()) bDestroy = true;

	if (bDestroy) {

		this->onDebugMessage("disconnecting because...");

		pSSLsocket->close();
		delete pSSLsocket;
		return;

	} // if destroy connection

	pSession = new WWWSsession(pSSLsocket, this);
	pSession->setMaxRequestSize(this->uiMaxRequestSize);
	pSession->uSessionID = this->uSessionCounter++;

	this->onDebugMessage("OK:Initiating connection "
						 + QString::number(pSession->uSessionID)
						 + " currently connected: "
						 + QString::number(this->apSessions.length()+1));

	this->addPendingConnection(pSSLsocket);

	connect(pSession, SIGNAL(debugMessage(QString)),
			this, SLOT(onDebugMessage(QString)));

	connect(pSession, SIGNAL(gotRequest(WWWSrequest*)),
			this, SLOT(onGotRequest(WWWSrequest*)));

	connect(pSession, SIGNAL(disconnected(WWWSsession*)),
			this, SLOT(onDisconnected(WWWSsession*)));

	this->apSessions.append(pSession);

	pSession->start();

} // incomingConnection


void WWWSserver::init(const QHostAddress oHost, const quint16 uiPort,
					  const QSslCertificate &oCert, const QSslKey &oKey) {

	this->oHost = oHost;
	this->uiPort = uiPort;

	this->oCertificate = oCert;
	this->oPrivateKey = oKey;

	//if (this->pTCPserver) return;

//	this->pTCPserver = new QTcpServer(this);

//	connect(this->pTCPserver, SIGNAL(newConnection()),
//			this, SLOT(onNewConnection()));

} // init


bool WWWSserver::isBlockedIP(const QString &sIP) {

	bool bOnList = this->asListBW.contains(sIP);

	// black list? -> block if on list
	if (this->bListIsBlack) return bOnList;

	// white list
	return !bOnList;

} // isBlockedIP


bool WWWSserver::isValidPath(const QString &sPath) {

	QString sBasePath;
	for (int i = 0; i < this->asListBasePaths.length(); ++i) {

		sBasePath = this->asListBasePaths.at(i);
		if (sPath.startsWith(sBasePath)) return true;

	} // loop

	return false;

} // isValidPath


void WWWSserver::onDisconnected(WWWSsession *pSession) {

	this->onDebugMessage("onDisconnected "
						 + QString::number(pSession->uSessionID));

	this->apSessions.removeOne(pSession);
	delete pSession;

} // onDisconnected


void WWWSserver::onNewConnection() {

	this->onDebugMessage("--------------onNewConnection----old attempt remove me-");
return;
	QTcpSocket *pTCPsocket;
	QSslSocket *pSSLsocket;
	WWWSsession *pSession;

	while (this->pTCPserver->hasPendingConnections()) {

		// get TCP socket
		pTCPsocket = this->pTCPserver->nextPendingConnection();

		// prepare SSL socket
		pSSLsocket = new QSslSocket(this);
		pSSLsocket->setLocalCertificate(this->oCertificate);
		pSSLsocket->setPrivateKey(this->oPrivateKey);
		pSSLsocket->setProtocol(QSsl::TlsV1SslV3);
		pSSLsocket->setPeerVerifyMode(QSslSocket::VerifyNone); // VerifyNone, QueryPeer, VerifyPeer, AutoVerifyPeer

		this->onDebugMessage("haha");
		if (!pSSLsocket->setSocketDescriptor(pTCPsocket->socketDescriptor())) {

			this->onDebugMessage(tr("KO:failed to hand over socket"));
			delete pSSLsocket;
			delete pTCPsocket;
			continue;

		} // if could not hand over socket discriptor

		pSession = new WWWSsession(pSSLsocket, this);

		connect(pSession, SIGNAL(debugMessage(QString)),
				this, SLOT(onDebugMessage(QString)));

		connect(pSession, SIGNAL(gotRequest(WWWSrequest*)),
				this, SLOT(onGotRequest(WWWSrequest*)));

		this->apSessions.append(pSession);
		pTCPsocket->setSocketDescriptor(0);

		pSession->start();

		this->onDebugMessage("hoho");

	} // loop this->pTCPserver->hasPendingConnections()
/*
	return;
	QTcpSocket *pSocket = this->pTCPserver->nextPendingConnection();

	this->onDebugMessage("New Connection: " + pSocket->peerAddress().toString()
						 + " " + pSocket->peerName() + " "
						 + QString::number(pSocket->peerPort(), 10)
						 + " " + pSocket->origin());

	WSSsession *pClient = new WSSsession(pSocket, this);

	connect(pClient, SIGNAL(disconnected()),
			this, SLOT(onSocketDisconnected()));

	connect(pClient, SIGNAL(debugMessage(QString)),
			this, SLOT(onDebugMessage(QString)));

	this->apSessions.append(pClient);
*/
} // onNewConnection


void WWWSserver::onSocketDisconnected() {

	this->onDebugMessage("Connection disconnected");

//	WSSsession *pClient = qobject_cast<WSSsession *>(sender());

//	if (!pClient) return;

//	this->apSessions.removeOne(pClient);
//	pClient->deleteLater();

} // onSocketDisconnected


void WWWSserver::onSSLerrors(const QList<QSslError> &aErrors) {

	this->onDebugMessage("SSL error(s)" + QString::number(aErrors.count()));

} // onSSLerrors


void WWWSserver::onOriginAuthenticationRequired(QWebSocketCorsAuthenticator *pWSCA) {
	Q_UNUSED(pWSCA)

	this->onDebugMessage("onOriginAuthenticationRequired");

} // onOriginAuthenticationRequired


void WWWSserver::onPeerVerifyError(QSslError oError) {
	Q_UNUSED(oError)

	this->onDebugMessage("onPeerVerifyError");

} // onPeerVerifyError


void WWWSserver::onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator *pSSLPSKA) {
	Q_UNUSED(pSSLPSKA)

	this->onDebugMessage("onPreSharedKeyAuthenticationRequired");

} // onPreSharedKeyAuthenticationRequired


void WWWSserver::onServerError(QWebSocketProtocol::CloseCode oCode) {
	Q_UNUSED(oCode)

	this->onDebugMessage("onServerError");

} // onServerError


void WWWSserver::respond(WWWSresponse *pResponse) {

	this->onDebugMessage("respond pResponse");

	connect(pResponse, SIGNAL(debugMessage(QString)),
			this, SLOT(onDebugMessage(QString)));

	if (pResponse->hasHeader("Server").isEmpty())
		pResponse->addHeader("Server", this->sServerString);// this->serverString());

	pResponse->startSending();

} // respond pResponse


void WWWSserver::respond(WWWSrequest *pRequest, const QString &sBody,
							 const quint16 uiCode) {

	this->onDebugMessage("respond pRequest sBody uiCode");

	WWWSresponse *pResponse = new WWWSresponse(pRequest);
	pResponse->setBody(sBody);
	// https://developer.mozilla.org/en-US/docs/Web/HTTP
	pResponse->setStatusCode(uiCode);

	this->respond(pResponse);

} // respond pRequest sBody uiCode


void WWWSserver::respond200(WWWSrequest *pRequest, const QString &sBody,
							const QString &sContentHeaderValue) {

	this->onDebugMessage("respond200");

	WWWSresponse *pResponse = new WWWSresponse(pRequest);
	pResponse->setBody(sBody);
	pResponse->addHeader("Content-Type", sContentHeaderValue);

	this->respond(pResponse);

} // respond200


void WWWSserver::respond404(WWWSrequest *pRequest) {

	this->onDebugMessage("respond404");

	QString sBody = "<html><head><title>404 Not Found</title></head><body>404 Not Found<body></html>";

	WWWSresponse *pResponse = new WWWSresponse(pRequest);
	pResponse->setStatusCode(404u);
	pResponse->setBody(sBody);

	this->respond(pResponse);

} // respond404


void WWWSserver::respond301(WWWSrequest *pRequest, const QString &sURL) {

	this->onDebugMessage("respond301");

	WWWSresponse *pResponse = new WWWSresponse(pRequest);
	pResponse->setStatusCode(301u);
	pResponse->addHeader("Location", sURL);

	this->respond(pResponse);

} // respond301


void WWWSserver::respondDirectoryList(WWWSrequest *pRequest, const QString &sPath) {

	if (!this->isValidPath(sPath)) {

		this->onDebugMessage("KO:Requesting non-authorized path: " + sPath);
		this->respond404(pRequest);
		return;

	} // if not an authorized path

	QString sOut = "<!DOCTYPE html>\r\n"
				   "<html>\n<head>\n"
				   "<title>QtSssShttps by Luke@SwissalpS.ws - HTTPS-Server</title>\n"
				   "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">"
				   "</head>\n<body style=\"background-color:tan;\">\n"
				   "<div id=\"dirList\" class=\"dirListDIV\">"
				   "<ol class=\"dirListOL\">";

	QString sBasePath = pRequest->targetOriginalQ().path();

	if (!sBasePath.endsWith("/")) sBasePath += "/";
	QString sName;
	QStringList aList = QDir(sPath).entryList(QDir::Dirs | QDir::Files
											  | QDir::NoSymLinks
											  | QDir::NoDotAndDotDot
											  | QDir::Readable);
	// NOTE: we limit directory length hardcoded here... maybe refactor...
	for (int i = 0; i < qMin(2000, aList.length()); ++i) {

		sName = aList.at(i);

		// don't list hidden items (should be dealt with by QDir::Filters)
		if (sName.startsWith(".")) continue;

		sOut += "<li class=\"dirListLI\">"
				"<a href=\"" + sBasePath + sName + "\">" + sName + "</a></li>";

	} // loop each entry

	sOut += "</ol></div>"
			"<div id=\"footerDiv\">"
			"<p></p></div>"
			"</body>\n</html>\n";

	this->respond200(pRequest, sOut);

} // respondDirList


void WWWSserver::respondFile(WWWSrequest *pRequest, const QString &sPathFile) {

	this->onDebugMessage("respondFile");

	QFile oFile(sPathFile);
	if (!oFile.open(QIODevice::ReadOnly)) {

		this->onDebugMessage(tr("Failed to open: ") + sPathFile);

		this->respond404(pRequest);

		return;

	} // if file did not open
	oFile.close();

	WWWSresponse *pResponse = new WWWSresponse(pRequest);
	pResponse->setFile(sPathFile);

	this->respond(pResponse);

} // respondFile


void WWWSserver::respondJSON(WWWSrequest *pRequest, const QString &sJSON) {

	this->onDebugMessage("respondJSON");

	WWWSresponse *pResponse = new WWWSresponse(pRequest);
	pResponse->setBody(sJSON);
	pResponse->addHeader("Content-Type", "application/json");

	this->respond(pResponse);

} // respondJSON


void WWWSserver::start() {

	this->onDebugMessage("start");

//	if (!this->pTCPserver) return;

	//this->pTCPserver->resumeAccepting();

	if (this->listen(this->oHost, this->uiPort)) {

		this->onDebugMessage(tr("OK:Listening at https://%1:%2")
							 .arg(oHost.toString()).arg(uiPort));

	} else {

		this->onDebugMessage(tr("KO:Could not start listening on port: %1 and IP: %2 error: %3")
							 .arg(uiPort).arg(oHost.toString())
							 .arg(this->pTCPserver->errorString()));

	} // if binding to addresss and port is OK

} // start


void WWWSserver::stop() {

	this->onDebugMessage("stop");

//	if (!this->pTCPserver) return;

//	this->pTCPserver->pauseAccepting();

} // stop



}	} // namespace SwissalpS::QtSssSapp

