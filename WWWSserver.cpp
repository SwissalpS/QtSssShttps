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


void WWWSserver::respond(WWWSresponse *pResponse) {

	this->onDebugMessage("respond pResponse");

	// TODO:

} // respond pResponse


void WWWSserver::respond(WWWSrequest *pRequest, QString &sBody,
							 const quint16 uiCode) {

	this->onDebugMessage("respond pRequest sBody uiCode");

	// TODO: select coresponding uiCode
	QString sCode = "OK";

	QString sHeader = "HTTP/1.1 %4 %5\r\n"
			  "Date: %1\r\n"
			  "Server: %2\r\n"
			  "Content-Type: text/html\r\n"
			  "Content-Encoding: utf8\r\n"
			  "Connection: close\r\n"
			  "Pragma: no-cache\r\n"
			  "Content-Length : %3"
			  "\r\n\r\n".arg(currentRFC7231date()).arg(this->sServerString)
					  .arg(sBody.length()).arg(uiCode).arg(sCode);

	QString sOut = sHeader + sBody;
	QSslSocket *pSocket = pRequest->socket();

	pSocket->write(sOut.toUtf8());
	pSocket->flush();
	pSocket->close();

} // respond pRequest sBody uiCode


void WWWSserver::respond200(WWWSrequest *pRequest, const QString &sBody,
							const QString &sContentHeaderValue) {

	QString sHeader = "HTTP/1.1 %4 %5\r\n"
			  "Date: %1\r\n"
			  "Server: %2\r\n"
			  "Content-Type: %6\r\n"
			  "Content-Encoding: utf8\r\n"
			  "Connection: close\r\n"
			  "Pragma: no-cache\r\n"
			  "Content-Length : %3"
			  "\r\n\r\n".arg(currentRFC7231date()).arg(this->sServerString)
					  .arg(sBody.length()).arg(200).arg("OK")
					  .arg(sContentHeaderValue);

	QString sOut = sHeader + sBody;
	QSslSocket *pSocket = pRequest->socket();

	pSocket->write(sOut.toUtf8());
	pSocket->flush();
	pSocket->close();

} // respond200


void WWWSserver::respond404(WWWSrequest *pRequest) {

	this->onDebugMessage("respond404");

	QString sBody = "<html><head><title>404 Not Found</title></head><body>404 Not Found<body></html>";
	QString sHeader = "HTTP/1.1 404 Not Found\r\n"
			  "Date: " + currentRFC7231date() + "\r\n"
			  "Server: SwissalpS WWWSserver\r\n"
			  "Content-Type: text/html\r\n"
			  "Content-Encoding: utf8\r\n"
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


void WWWSserver::respond301(WWWSrequest *pRequest, const QString &sURL) {

	QString sOut = "HTTP/1.1 %4 %5\r\n"
			  "Date: %1\r\n"
			  "Server: %2\r\n"
			  "Location: %6\r\n"
			  "Connection: close\r\n"
			  "Pragma: no-cache\r\n"
			  "Content-Length : %3"
			  "\r\n\r\n".arg(currentRFC7231date()).arg(this->sServerString)
					  .arg(0).arg(301).arg("Moved Permanently").arg(sURL);

	QSslSocket *pSocket = pRequest->socket();

	pSocket->write(sOut.toUtf8());
	pSocket->flush();
	pSocket->close();

} // respond301


void WWWSserver::respondDirList(WWWSrequest *pRequest, const QString &sPath) {

	QString sOut = "<!DOCTYPE html>\r\n"
				   "<html>\n<head>\n"
				   "<title>QtSssShttps by Luke@SwissalpS.ws - HTTPS-Server</title>\n"
				   "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">"
				   "</head>\n<body style=\"background-color:tan;\">\n"
				   "<div id=\"dirList\" class=\"dirListDIV\">"
				   "<ol class=\"dirListOL\">";

	QString sBasePath = QUrl(pRequest->request()).toString(QUrl::RemoveQuery
														   | QUrl::RemoveScheme);
	if (!sBasePath.endsWith("/")) sBasePath += "/";
	QString sName;
	QStringList aList = QDir(sPath).entryList();
	for (int i = 0; i < aList.lenghth(); ++i) {

		sName = aList.at(i);

		// don't list hidden items
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

	QMimeDatabase oMdb;
	QString sMimeType = oMdb.mimeTypeForFile(sPathFile).name();

	QByteArray aFile;
	QFile oFile(sPathFile);

	if (!oFile.open(QIODevice::ReadOnly)) {

		this->onDebugMessage(tr("Failed to read: ") + sPathFile);

		this->respond404(pRequest);

		return;

	} // if file did not open

	// \r needs to be before \n
	QString sHeader = "HTTP/1.1 %4 %5\r\n"
			  "Date: %1\r\n"
			  "Server: %2\r\n"
			  "Content-Type: %6\r\n"
			  "Content-Encoding: utf8\r\n"
			  "Connection: close\r\n"
			  "Pragma: no-cache\r\n"
			  "Content-Length : %3"
			  "\r\n\r\n".arg(currentRFC7231date()).arg(this->sServerString)
					  .arg(oFile.size()).arg(200).arg("OK")
					  .arg(sMimeType);

	QSslSocket *pSocket = pRequest->socket();

	pSocket->write(sHeader.toUtf8());

	int iBufferSize = 16000;
	aFile.resize(iBufferSize);
	qint64 iRead;

	while (!oFile.atEnd()) {

		iRead = oFile.read(aFile.data(), iBufferSize);

		// If an error occurred during the read, emit an error
		if (-1 == iRead) {

			this->onDebugMessage(tr("KO:error reading file. ")
								 + oFile.errorString());

			pSocket->flush();
			pSocket->close();

			return;

		} // if error reading

		// Write the data to the destination device
		if (-1 == pSocket->write(aFile.constData(), iRead)) {

			this->onDebugMessage(tr("KO:error writing to client. ")
								+ pSocket->errorString());

			pSocket->flush();
			pSocket->close();

			return;

		} // if error writing

	} // loop while !oFile.atEnd()

	pSocket->flush();
	pSocket->close();

	// TODO: implement something like bellow to keep interface responsive
//    // Check if the end of the device has been reached - if so,
//    // emit the finished signal and if not, continue to read
//    // data at the next iteration of the event loop
//    if(src->atEnd()) {
//        Q_EMIT q->finished();
//    } else {
//        QTimer::singleShot(0, this, SLOT(nextBlock()));
//    }

} // respondFile


void WWWSserver::respondJSON(WWWSrequest *pRequest, const QString &sJSON) {

	QString sHeader = "HTTP/1.1 %4 %5\r\n"
			  "Date: %1\r\n"
			  "Server: %2\r\n"
			  "Content-Type: %6\r\n"
			  "Content-Encoding: utf8\r\n"
			  "Connection: close\r\n"
			  "Pragma: no-cache\r\n"
			  "Content-Length : %3"
			  "\r\n\r\n".arg(currentRFC7231date()).arg(this->sServerString)
					  .arg(sBody.length()).arg(200).arg("OK")
					  .arg("application/json");

	QString sOut = sHeader + sJSON;
	QSslSocket *pSocket = pRequest->socket();

	pSocket->write(sOut.toUtf8());
	pSocket->flush();
	pSocket->close();

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

