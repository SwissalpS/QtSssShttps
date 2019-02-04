#include "WSSserver.h"
#include "AppController.h"
#include "AppSettings.h"
#include "WSSsession.h"

#include <QFile>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>



namespace SwissalpS { namespace QtSssSapp {



WSSserver::WSSserver(QObject *pParent) :
	QObject(pParent),
	bListIsBlack(false),
	pWebSocketServer(nullptr),
	uiMaxConnections(25u),
	uiPort(0u) {

	this->asListBW.clear();
	this->apClients.clear();
	this->apSessions.clear();

} // construct


WSSserver::~WSSserver() {

	this->pWebSocketServer->close();
	qDeleteAll(this->apSessions);

} // dealloc


void WSSserver::init(QHostAddress oHost, const quint16 uiPort,
					 const QSslCertificate &oCert, const QSslKey &oKey) {

	this->oHost = oHost;
	this->uiPort = uiPort;

	QSslConfiguration oSSLconfig;

	oSSLconfig.setPeerVerifyMode(QSslSocket::VerifyNone); // VerifyNone, QueryPeer, VerifyPeer, AutoVerifyPeer
	oSSLconfig.setLocalCertificate(oCert);
	oSSLconfig.setPrivateKey(oKey);
	oSSLconfig.setProtocol(QSsl::TlsV1SslV3);

	this->pWebSocketServer = new QWebSocketServer(SssS_APP_NAME_FULL,
												  QWebSocketServer::SecureMode,
												  this);

	this->pWebSocketServer->setSslConfiguration(oSSLconfig);

} // init


bool WSSserver::isBlockedIP(const QString &sIP) {

	bool bOnList = this->asListBW.contains(sIP);

	// black list? -> block if on list
	if (this->bListIsBlack) return bOnList;

	// white list
	return !bOnList;

} // isBlockedIP


void WSSserver::onNewConnection() {

	this->onDebugMessage("onNewConnection");

	QWebSocket *pSocket;

	while (this->pWebSocketServer->hasPendingConnections()) {

		pSocket = this->pWebSocketServer->nextPendingConnection();

		bool bDestroy = false;
		if (this->isBlockedIP(pSocket->peerAddress().toString())) bDestroy = true;
		if (this->uiMaxConnections <= this->apSessions.length()) bDestroy = true;

		if (bDestroy) {

			this->onDebugMessage("ignorinig connection");

			pSocket->close();
			delete pSocket;
			return;

		} // if destroy connection

		this->onDebugMessage("New Connection: " + pSocket->peerAddress().toString()
							 + " " + pSocket->peerName() + " "
							 + QString::number(pSocket->peerPort(), 10)
							 + " " + pSocket->origin());

		WSSsession *pClient = new WSSsession(pSocket, this);

		connect(pClient, SIGNAL(disconnected()),
				this, SLOT(onSocketDisconnected()));

		connect(pClient, SIGNAL(debugMessage(QString)),
				this, SLOT(onDebugMessage(QString)));

		connect(pClient, SIGNAL(gotJSON(QString,QJsonObject,WSSsession*)),
				this, SLOT(onGotJSON(QString,QJsonObject,WSSsession*)));

		this->apSessions.append(pClient);

	} // loop this->pWebSocketServer->hasPendingConnections()

} // onNewConnection


void WSSserver::onSocketDisconnected() {

	this->onDebugMessage("Connection disconnected");

	WSSsession *pClient = qobject_cast<WSSsession *>(sender());

	if (!pClient) return;

	this->apSessions.removeOne(pClient);
	pClient->deleteLater();

} // onSocketDisconnected


void WSSserver::onSSLerrors(const QList<QSslError> &aErrors) {

	this->onDebugMessage("SSL error(s)" + QString::number(aErrors.count()));

} // onSSLerrors


void WSSserver::onOriginAuthenticationRequired(QWebSocketCorsAuthenticator *pWSCA) {
	Q_UNUSED(pWSCA)

	this->onDebugMessage("onOriginAuthenticationRequired");

} // onOriginAuthenticationRequired


void WSSserver::onPeerVerifyError(QSslError oError) {
	Q_UNUSED(oError)

	this->onDebugMessage("onPeerVerifyError");

} // onPeerVerifyError


void WSSserver::onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator *pSSLPSKA) {
	Q_UNUSED(pSSLPSKA)

	this->onDebugMessage("onPreSharedKeyAuthenticationRequired");

} // onPreSharedKeyAuthenticationRequired


void WSSserver::onServerError(QWebSocketProtocol::CloseCode oCode) {
	Q_UNUSED(oCode)

	this->onDebugMessage("onServerError");

} // onServerError


void WSSserver::start() {

	this->onDebugMessage("start");

	if (this->pWebSocketServer->listen(this->oHost, this->uiPort)) {

		this->onDebugMessage("OK:Listening on port: "
							 + QString::number(this->uiPort)
							 + " and IP: " + this->oHost.toString()
							 + " " + this->pWebSocketServer->serverUrl().toString());

		connect(this->pWebSocketServer, SIGNAL(newConnection()),
				this, SLOT(onNewConnection()));

		connect(this->pWebSocketServer, SIGNAL(sslErrors(QList<QSslError>)),
				this, SLOT(onSSLerrors(QList<QSslError>)));

		connect(this->pWebSocketServer, SIGNAL(originAuthenticationRequired(QWebSocketCorsAuthenticator*)),
				this, SLOT(onOriginAuthenticationRequired(QWebSocketCorsAuthenticator*)));

		connect(this->pWebSocketServer, SIGNAL(peerVerifyError(QSslError)),
				this, SLOT(onPeerVerifyError(QSslError)));

		connect(this->pWebSocketServer, SIGNAL(preSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator*)),
				this, SLOT(onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator*)));

		connect(this->pWebSocketServer, SIGNAL(serverError(QWebSocketProtocol::CloseCode)),
				this, SLOT(onServerError(QWebSocketProtocol::CloseCode)));

	} else {

		this->onDebugMessage("KO:Could not start listening on port: "
							 + QString::number(uiPort)
							 + " and IP: " + oHost.toString()
							 + " error: " + this->pWebSocketServer->errorString());

		//AppController::pAppController()->quit();

	} // if binding to addresss and port is OK

} // start


void WSSserver::stop() {

	this->onDebugMessage("stop");

} // stop



}	} // namespace SwissalpS::QtSssSapp

