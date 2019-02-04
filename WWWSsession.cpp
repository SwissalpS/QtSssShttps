#include "WWWSsession.h"

#include <QFile>
#include <QDateTime>

#include "WWWSrequest.h"



namespace SwissalpS { namespace QtSssSapp {



WWWSsession::WWWSsession(QSslSocket *pSocket, QObject *pParent) :
	QObject(pParent),
	pSocket(pSocket),
	pRequest(nullptr) {

	if (nullptr == pSocket) {

		qFatal("KO::Passed null QWebSocket to WWWSsession constructor!");

		return;

	} // if null passed

//	connect(this->pSocket, SIGNAL(aboutToClose()),
//			this, SLOT(onAboutToClose));

//	connect(this->pSocket, SIGNAL(bytesWritten(qint64)),
//			this, SLOT(onBytesWritten(qint64)));

//	connect(this->pSocket, SIGNAL(channelBytesWritten(int,qint64)),
//			this, SLOT(onChannelBytesWritten(int,qint64)));

	connect(this->pSocket, SIGNAL(channelReadyRead(int)),
			this, SLOT(onChannelReadyRead(int)));

	connect(this->pSocket, SIGNAL(connected()),
			this, SLOT(onConnected()));

	connect(this->pSocket, SIGNAL(disconnected()),
			this, SLOT(onDisconnected()));

	connect(this->pSocket, SIGNAL(encrypted()),
			this, SLOT(onEncrypted()));

	connect(this->pSocket, SIGNAL(encryptedBytesWritten(qint64)),
			this, SLOT(onEncryptedBytesWritten(qint64)));

	connect(this->pSocket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(onError(QAbstractSocket::SocketError)));

//	connect(this->pSocket, SIGNAL(hostFound()),
//			this, SLOT(onHostFound()));

	connect(this->pSocket, SIGNAL(modeChanged(QSslSocket::SslMode)),
			this, SLOT(onModeChanged(QSslSocket::SslMode)));

	connect(this->pSocket, SIGNAL(peerVerifyError(QSslError)),
			this, SLOT(onPeerVerifyError(QSslError)));

	connect(this->pSocket, SIGNAL(preSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator*)),
			this, SLOT(onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator*)));

	connect(this->pSocket, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
			this, SLOT(onProxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));

	connect(this->pSocket, SIGNAL(readChannelFinished()),
			this, SLOT(onReadChannelFinished()));

	connect(this->pSocket, SIGNAL(readyRead()),
			this, SLOT(onReadyRead()));

	connect(this->pSocket, SIGNAL(sslErrors(QList<QSslError>)),
			this, SLOT(onSSLerrors(QList<QSslError>)));

	connect(this->pSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
			this, SLOT(onStateChanged(QAbstractSocket::SocketState)));

} // construct


WWWSsession::~WWWSsession() {

	if (this->pSocket) this->pSocket->deleteLater();
	if (this->pRequest) delete this->pRequest;

} // dealloc


QString WWWSsession::currentRFC7231date() {

	return QDateTime::currentDateTimeUtc().toString("ddd, d MMM yyyy hh:mm:ss") + " GMT";

} // currentRFC7231date


void WWWSsession::start() {

	this->onDebugMessage("start");

	this->pSocket->startServerEncryption();

} // start


void WWWSsession::onChannelReadyRead(const int iChannel) {

	this->onDebugMessage("onChannelReadyRead " + QString::number(iChannel));

} // onChannelReadyRead


void WWWSsession::onConnected() {

	this->onDebugMessage("onConnected");

} // onConnected


void WWWSsession::onEncrypted() {

	this->onDebugMessage("onEncrypted");

} // onEncrypted


void WWWSsession::onEncryptedBytesWritten(const qint64 illCount) {

	this->onDebugMessage("onEncryptedBytesWritten " + QString::number(illCount));


} // onEncryptedBytesWritten


void WWWSsession::onError(const QAbstractSocket::SocketError eError) {

	QString sMessage = "onError " + QString::number(eError) + " ";

	switch (eError) {

		case QAbstractSocket::ConnectionRefusedError:
			sMessage += "ConnectionRefusedError"; break;
		case QAbstractSocket::RemoteHostClosedError:
			sMessage += "RemoteHostClosedError"; break;
		case QAbstractSocket::HostNotFoundError:
			sMessage += "HostNotFoundError"; break;
		case QAbstractSocket::SocketAccessError:
			sMessage += "SocketAccessError"; break;
		case QAbstractSocket::SocketResourceError:
			sMessage += "SocketResourceError"; break;
		case QAbstractSocket::SocketTimeoutError:
			sMessage += "SocketTimeoutError"; break;
		case QAbstractSocket::DatagramTooLargeError:
			sMessage += "DatagramTooLargeError"; break;
		case QAbstractSocket::NetworkError:
			sMessage += "NetworkError"; break;
		case QAbstractSocket::AddressInUseError:
			sMessage += "AddressInUseError"; break;
		case QAbstractSocket::SocketAddressNotAvailableError:
			sMessage += "SocketAddressNotAvailableError"; break;
		case QAbstractSocket::UnsupportedSocketOperationError:
			sMessage += "UnsupportedSocketOperationError"; break;
		case QAbstractSocket::UnfinishedSocketOperationError:
			sMessage += "UnfinishedSocketOperationError"; break;
		case QAbstractSocket::ProxyAuthenticationRequiredError:
			sMessage += "ProxyAuthenticationRequiredError"; break;
		case QAbstractSocket::SslHandshakeFailedError:
			sMessage += "SslHandshakeFailedError"; break;
		case QAbstractSocket::ProxyConnectionRefusedError:
			sMessage += "ProxyConnectionRefusedError"; break;
		case QAbstractSocket::ProxyConnectionClosedError:
			sMessage += "ProxyConnectionClosedError"; break;
		case QAbstractSocket::ProxyConnectionTimeoutError:
			sMessage += "ProxyConnectionTimeoutError"; break;
		case QAbstractSocket::ProxyNotFoundError:
			sMessage += "ProxyNotFoundError"; break;
		case QAbstractSocket::ProxyProtocolError:
			sMessage += "ProxyProtocolError"; break;
		case QAbstractSocket::OperationError:
			sMessage += "OperationError"; break;
		case QAbstractSocket::SslInternalError:
			sMessage += "SslInternalError"; break;
		case QAbstractSocket::SslInvalidUserDataError:
			sMessage += "SslInvalidUserDataError"; break;
		case QAbstractSocket::TemporaryError:
			sMessage += "TemporaryError"; break;
		default:
			sMessage += "Unknown";
		break;
	} // switch eMode

	this->onDebugMessage(sMessage);

} // onError


void WWWSsession::onModeChanged(const QSslSocket::SslMode eMode) {

	QString sMessage = "onModeChanged " + QString::number(eMode) + " ";

	switch (eMode) {
		case QSslSocket::UnencryptedMode:
			sMessage += "UnencryptedMode";
		break;
		case QSslSocket::SslClientMode:
			sMessage += "SslClientMode";
		break;
		case QSslSocket::SslServerMode:
			sMessage += "SslServerMode";
		break;
		default:
			sMessage += "Unknown";
		break;
	} // switch eMode

	this->onDebugMessage(sMessage);

} // onModeChanged


void WWWSsession::onPeerVerifyError(const QSslError oError) {
	Q_UNUSED(oError)

	this->onDebugMessage("onPeerVerifyError");

} // onPeerVerifyError


void WWWSsession::onPreSharedKeyAuthenticationRequired(
		QSslPreSharedKeyAuthenticator *pPSKauthenticator) {
	Q_UNUSED(pPSKauthenticator)

	this->onDebugMessage("onPreSharedKeyAuthenticationRequired");

} // onPreSharedKeyAuthenticationRequired


void WWWSsession::onProxyAuthenticationRequired(QNetworkProxy oProxy,
												QAuthenticator *pAuthenticator) {
	Q_UNUSED(oProxy)
	Q_UNUSED(pAuthenticator)

	this->onDebugMessage("onProxyAuthenticationRequired");


} // onProxyAuthenticationRequired


void WWWSsession::onReadChannelFinished() {

	this->onDebugMessage("onReadChannelFinished");

} // onReadChannelFinished


void WWWSsession::onReadyRead() {

	this->onDebugMessage("onReadyRead");

	if (!this->pSocket->isEncrypted()) {

		// TODO:
		this->onDebugMessage("KO:Socket is not encrypted!");
		return;

	} // if not encrypted

	QByteArray aubRaw = this->pSocket->readAll();
	WWWSrequest *pRequest = new WWWSrequest(aubRaw, this);

	if (pRequest->isNull()) {

		this->onDebugMessage("KO:Null-request!");
		delete pRequest;
		this->pSocket->close();

		return;

	} // if null request

	this->pRequest = pRequest;
	Q_EMIT this->gotRequest(pRequest);

} // onReadyRead


void WWWSsession::onSSLerrors(const QList<QSslError> aoErrors) {

	this->onDebugMessage("onSSLerrors " + QString::number(aoErrors.length()));

} // onSSLerrors


void WWWSsession::onStateChanged(const QAbstractSocket::SocketState eState) {

	QString sMessage = "onStateChanged " + QString::number(eState) + " ";

	switch (eState) {
		case QAbstractSocket::UnconnectedState:
			sMessage += "UnconnectedState";
		break;
		case QAbstractSocket::HostLookupState:
			sMessage += "HostLookupState";
		break;
		case QAbstractSocket::ConnectingState:
			sMessage += "ConnectingState";
		break;
		case QAbstractSocket::ConnectedState:
			sMessage += "ConnectedState";
		break;
		case QAbstractSocket::BoundState:
			sMessage += "BoundState";
		break;
		case QAbstractSocket::ListeningState:
			sMessage += "ListeningState";
		break;
		case QAbstractSocket::ClosingState:
			sMessage += "ClosingState";
		break;
		default:
			sMessage += "Unknown state";
		break;
	} // switch eState

	this->onDebugMessage(sMessage);

} // onStateChanged



}	} // namespace SwissalpS::QtSssSapp

