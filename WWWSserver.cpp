#include "WWWSserver.h"

#include <QDateTime>
#include <QFile>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>



namespace SwissalpS { namespace QtSssSapp {



WWWSserver::WWWSserver(QObject *pParent) :
	QTcpServer(pParent),
	bListIsBlack(false),
	pTCPserver(nullptr),
	sServerString("SwissalpS WWWSserver"),
	uiMaxConnections(25u),
	uiPort(0u) {

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


void WWWSserver::incomingConnection(qintptr ipSocketDescriptor) {

	QSslSocket *pSSLsocket;
	WWWSsession *pSession;

	// prepare SSL socket
	pSSLsocket = new QSslSocket(this);
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

	this->addPendingConnection(pSSLsocket);

	pSession = new WWWSsession(pSSLsocket, this);

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


void WWWSserver::onDisconnected(WWWSsession *pSession) {

	this->apSessions.removeOne(pSession);
	delete pSession;

} // onDisconnected


void WWWSserver::onNewConnection() {

	this->onDebugMessage("--------------onNewConnection-----");

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


void WWWSserver::respond404(WWWSrequest *pRequest) {

	this->onDebugMessage("respond404");

	QString sBody = "<html><head><title>404 Not Found</title></head><body>404 Not Found<body></html>";
	QString sHeader = "HTTP/1.1 404 Not Found\r\n"
			  "Date: " + currentRFC7231date() + "\r\n"
			  "Server: SwissalpS WWWSserver\r\n"
			  "Content-Type: text/html\r\n"
			  "Connection: close\r\n"
			  "Pragma: no-cache\r\n"
			  "Content-Length : " + QString::number(sBody.length())
			  + "\r\n\r\n";

	QString sOut = sHeader + sBody;
	QSslSocket *pSocket = pRequest->socket();

	pSocket->write(sOut.toUtf8());
	pSocket->flush();
	pSocket->close();

} // respond404


void WWWSserver::respondPage(WWWSrequest *pRequest, QString &sBody) {

	this->onDebugMessage("respondPage");

	QString sHeader = "HTTP/1.1 200 OK\r\n"
			  "Date: " + currentRFC7231date() + "\r\n"
			  "Server: " + this->sServerString + "\r\n"
			  "Content-Type: text/html\r\n"
			  "Connection: close\r\n"
			  "Pragma: no-cache\r\n"
			  "Content-Length : " + QString::number(sBody.length())
			  + "\r\n\r\n";

	QString sOut = sHeader + sBody;
	QSslSocket *pSocket = pRequest->socket();

	pSocket->write(sOut.toUtf8());
	pSocket->flush();
	pSocket->close();

} // respondPage


void WWWSserver::start() {

	this->onDebugMessage("start");

//	if (!this->pTCPserver) return;

	//this->pTCPserver->resumeAccepting();

	if (this->listen(this->oHost, this->uiPort)) {

		this->onDebugMessage("OK:Listening on port: "
							 + QString::number(uiPort)
							 + " and IP: " + oHost.toString());

	} else {

		this->onDebugMessage("KO:Could not start listening on port: "
							 + QString::number(uiPort)
							 + " and IP: " + oHost.toString()
							 + " error: " + this->pTCPserver->errorString());

	} // if binding to addresss and port is OK

} // start


void WWWSserver::stop() {

	this->onDebugMessage("stop");

//	if (!this->pTCPserver) return;

//	this->pTCPserver->pauseAccepting();

} // stop



}	} // namespace SwissalpS::QtSssSapp

