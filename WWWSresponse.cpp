#include "WWWSresponse.h"

#include <QMimeDatabase>



namespace SwissalpS { namespace QtSssSapp {



WWWSresponse::WWWSresponse(WWWSrequest *pRequestParent) :
	QObject(pRequestParent),
	pRequest(pRequestParent),
	sBody(""),
	sPathFile(""),
	uiStatusCode(200u),
	eTT(SendHeadersOnly),
	iTimer(-1),
	iCount(0),
	iContentLength(0),
	iBufferSize(16000) {

	this->hspHeaders.clear();

} // construct


WWWSresponse::~WWWSresponse() {

	this->hspHeaders.clear();
	this->pRequest = nullptr;

} // dealloc


void WWWSresponse::addHeader(WWWheader *pHeader) {

	if (pHeader->isNull()) return;

	QString sKey = this->hasHeader(pHeader->title());
	if (sKey.isEmpty())
		this->hspHeaders.insert(pHeader->title(), pHeader);
	else
		this->hspHeaders.value(sKey)->setValue(pHeader->value());

} // addHeader pHeader


void WWWSresponse::addHeader(const QString &sTitle, const QString sValue) {

	this->addHeader(new WWWheader(sTitle, sValue, this));

} // addHeader sTitle sValue


QString WWWSresponse::currentRFC7231date() {

	return QDateTime::currentDateTimeUtc().toString("ddd, d MMM yyyy hh:mm:ss") + " GMT";

} // currentRFC7231date


void WWWSresponse::dropHeader(const QString &sTitle) {

	QString sKey = this->hasHeader(sTitle);
	if (sKey.isEmpty()) return;

	WWWheader *pHeader = this->hspHeaders.take(sKey);
	delete pHeader;

} // dropHeader


void WWWSresponse::finalize() {

	this->onDebugMessage("finalize");

	if (!this->sPathFile.isEmpty()) {

		this->eTT = SendFile;

		this->oFile.setFileName(this->sPathFile);
		if (!this->oFile.open(QIODevice::ReadOnly)) {

			this->onDebugMessage(tr("Failed to open: ") + this->sPathFile);

			// TODO: emit some signal to have server redirect to 404
			//this->respond404(pRequest);
			this->sBody = "";
			this->eTT = SendText;
			this->addHeader("Content-Type", "text/html");
			this->uiStatusCode = 404;

		} else {

			QMimeDatabase oMdb;
			const QString sMimeType = oMdb.mimeTypeForFile(sPathFile).name();
			this->addHeader("Content-Type", sMimeType);

		} // if file did not open

	} else if (this->sBody.isEmpty())
		this->eTT = SendHeadersOnly;
	else
		this->eTT = SendText;

	if (this->hasHeader("Connection").isEmpty())
		this->addHeader("Connection", "close");

	if (this->hasHeader("Content-Encoding").isEmpty())
		this->addHeader("Content-Encoding", "utf8");

	if (this->hasHeader("Content-Type").isEmpty())
		this->addHeader("Content-Type", "text/html");

	if (this->hasHeader("Pragma").isEmpty())
		this->addHeader("Pragma", "no-cache");

	if (this->hasHeader("Server").isEmpty())
		this->addHeader("Server", "SwissalpS WWWSresponse");

	switch (this->eTT) {

		case SendHeadersOnly:
			this->iContentLength = 0;
		break;

		case SendFile:
			this->iContentLength = oFile.size();
		break;

		default:
		case SendText:
			this->iContentLength = this->sBody.length();
		break;

	} // switch this->eTT

	// very large file or stream
	if (0 > this->iContentLength)
		this->dropHeader("Content-Length");
	else
		this->addHeader("Content-Length", QString::number(this->iContentLength));

} // finalize


QString WWWSresponse::fullHeaderString() {

	this->onDebugMessage("fullHeaderString");

	this->addHeader("Date", WWWSresponse::currentRFC7231date());

	QString sHeader = QString("HTTP/1.1 %1 %2\r\n").arg(this->uiStatusCode)
					  .arg(WWWSresponse::stringForStatusCode(this->uiStatusCode));

	QStringList asKeys = this->hspHeaders.keys();
	WWWheader *pHeader;
	for (int i = 0; i < asKeys.length(); ++i) {

		pHeader = this->hspHeaders.value(asKeys.at(i));
		sHeader += pHeader->toString() + "\r\n";
		//this->onDebugMessage(pHeader->toString());

	} // loop

	sHeader += "\r\n";

	return sHeader;

} // fullHeaderString


QString WWWSresponse::hasHeader(const QString &sTitle) const {

	QStringList asKeys = this->hspHeaders.keys();
	if (!asKeys.contains(sTitle, Qt::CaseInsensitive)) return QString();

	for (int i = 0; i < asKeys.length(); ++i) {

		if (0 == sTitle.compare(asKeys.at(i), Qt::CaseInsensitive)) {

			return asKeys.at(i);

		} // if found match

	} // loop

	return QString();

} // hasHeader


void WWWSresponse::onDie() {

	if (-1 != this->iTimer) {

		this->killTimer(this->iTimer);
		this->iTimer = -1;

	} // if have timer

	if (this->oFile.isOpen()) this->oFile.close();
	this->disconnect(&this->oFile);

	if (nullptr == this->pRequest) return;
	QSslSocket *pSocket = this->pRequest->socket();
	this->pRequest = nullptr;
	if (!pSocket->isValid()) return;

	this->disconnect(pSocket);

	pSocket->flush();
	pSocket->close();

	this->deleteLater();

} // onDie


void WWWSresponse::onSequentialFileReadyRead() {

	this->onDebugMessage("onSequentialFileReadyRead");

	QSslSocket *pSocket = this->pRequest->socket();

	QByteArray aFile;
	aFile.resize(this->iBufferSize);
	qint64 iRead = this->oFile.read(aFile.data(), this->iBufferSize);

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

} // onSequentialFileReadyRead


void WWWSresponse::startSending() {

	this->onDebugMessage("startSending");

	if (nullptr == this->pRequest) {

		this->onDebugMessage("KO:no pRequest");

		this->deleteLater();
		return;

	} // if no request -> no socket

	QSslSocket *pSocket = pRequest->socket();
	if (!pSocket->isValid()) {

		this->onDebugMessage("KO:socket is invalid");

		this->deleteLater();
		return;

	} // if not a valid socket

	connect(pSocket, SIGNAL(destroyed(QObject*)),
			this, SLOT(onDie()));

	connect(&this->oFile, SIGNAL(destroyed(QObject*)),
			this, SLOT(onDie()));

	// make sure certain headers are set
	this->finalize();

	QString sOut = this->fullHeaderString();

	if (0 > this->iContentLength) {
		// just make sure it doesn't count as small
	} else if (this->iContentLength <= this->iBufferSize) {

		this->onDebugMessage("OK:Is small size");

		if (WWWSresponse::SendFile == this->eTT) {

			sOut += QString(oFile.readAll());
			oFile.close();

		} else {

			sOut += this->sBody;

		} // switch type

		pSocket->write(sOut.toUtf8());
		pSocket->flush();
		// TODO: respect header
		this->onDebugMessage("OK:about to close socket");
		pSocket->close();
		// this causes seg-fault
		//this->onDebugMessage("OK:about to disconnect socket");
		// does not seem to be required
		//pSocket->disconnect();
		return;

	} // if small load

	this->onDebugMessage("OK:starting bigger upload");

	this->iCount = 0;

	// send header
	pSocket->write(sOut.toUtf8());

	if ((WWWSresponse::SendFile == this->eTT)
			&& (this->oFile.isSequential())) {

		connect(&this->oFile, SIGNAL(readyRead()),
				this, SLOT(onSequentialFileReadyRead()));

		connect(&this->oFile, SIGNAL(readChannelFinished()),
				this, SLOT(onDie()));

	} else {

		// start the timer to kick off sending
		this->iTimer = this->startTimer(0);

	} // if sequential file or other type

} // startSending


QString WWWSresponse::stringForStatusCode(const quint16 uiCode) {

	switch (uiCode) {

		case 100: return QStringLiteral("Continue");
		case 101: return QStringLiteral("Switching Protocols");
		case 200: return QStringLiteral("OK");
		case 201: return QStringLiteral("Created");
		case 202: return QStringLiteral("Accepted");
		case 203: return QStringLiteral("Non-Authoritative Information");
		case 204: return QStringLiteral("No Content");
		case 205: return QStringLiteral("Reset Content");
		case 206: return QStringLiteral("Partial Content");
		case 300: return QStringLiteral("Multiple Choices");
		case 301: return QStringLiteral("Moved Permanently");
		case 302: return QStringLiteral("Found");
		case 303: return QStringLiteral("See Other");
		case 304: return QStringLiteral("Not Modified");
		case 307: return QStringLiteral("Temporary Redirect");
		case 308: return QStringLiteral("Permanent Redirect");
		case 400: return QStringLiteral("Bad Request");
		case 401: return QStringLiteral("Unauthorized");
		case 403: return QStringLiteral("Forbidden");
		case 404: return QStringLiteral("Not Found");
		case 405: return QStringLiteral("Method Not Allowed");
		case 406: return QStringLiteral("Not Acceptable");
		case 407: return QStringLiteral("Proxy Authentication Required");
		case 408: return QStringLiteral("Request Timeout");
		case 409: return QStringLiteral("Conflict");
		case 410: return QStringLiteral("Gone");
		case 411: return QStringLiteral("Length Required");
		case 412: return QStringLiteral("Precondition Failed");
		case 413: return QStringLiteral("Payload Too Large");
		case 414: return QStringLiteral("URI Too Long");
		case 415: return QStringLiteral("Unsupported Media Type");
		case 416: return QStringLiteral("Range Not Satisfiable");
		case 417: return QStringLiteral("Expectation Failed");
		case 418: return QStringLiteral("I'm a teapot");
		case 420: return QStringLiteral("Four Twenty");
		case 422: return QStringLiteral("Unprocessable Entity");
		case 425: return QStringLiteral("Too Early");
		case 426: return QStringLiteral("Upgrade Required");
		case 428: return QStringLiteral("Precondition Required");
		case 429: return QStringLiteral("Too Many Requests");
		case 431: return QStringLiteral("Request Header Fields Too Large");
		case 451: return QStringLiteral("Unavailable For Legal Reasons");
		case 500: return QStringLiteral("Internal Server Error");
		case 501: return QStringLiteral("Not Implemented");
		case 502: return QStringLiteral("Bad Gateway");
		case 503: return QStringLiteral("Service Unavailable");
		case 504: return QStringLiteral("Gateway Timeout");
		case 505: return QStringLiteral("HTTP Version Not Supported");
		case 511: return QStringLiteral("Network Authentication Required");

		default: return QString::number(uiCode);

	} // switch uiCode

} // stringForCode


void WWWSresponse::timerEvent(QTimerEvent *pEvent) {

	//this->onDebugMessage("timerEvent");

	pEvent->accept();

	QSslSocket *pSocket = pRequest->socket();

	if (WWWSresponse::SendFile == this->eTT) {

		if (this->oFile.atEnd()) {

			this->killTimer(this->iTimer);
			this->iTimer = -1;

			oFile.close();

			pSocket->flush();
			pSocket->close();

		} else {

			QByteArray aFile;
			aFile.resize(this->iBufferSize);
			qint64 iRead = this->oFile.read(aFile.data(), this->iBufferSize);

			// If an error occurred during the read, emit an error
			if (-1 == iRead) {

				this->killTimer(this->iTimer);
				this->iTimer = -1;

				this->onDebugMessage(tr("KO:error reading file. ")
									 + oFile.errorString());

				pSocket->flush();
				pSocket->close();

				return;

			} // if error reading

			// Write the data to the destination device
			if (-1 == pSocket->write(aFile.constData(), iRead)) {

				this->killTimer(this->iTimer);
				this->iTimer = -1;

				this->onDebugMessage(tr("KO:error writing to client. ")
									+ pSocket->errorString());

				pSocket->flush();
				pSocket->close();

				return;

			} // if error writing

		} // if still got data or not

	} else {

		if (this->sBody.length() < this->iCount) {

			this->killTimer(this->iTimer);
			this->iTimer = -1;

			oFile.close();

			pSocket->flush();
			pSocket->close();

			return;

		} // if pointer points beyond size -> done

		// write next portion
		pSocket->write(this->sBody.mid(this->iCount, this->iBufferSize).toUtf8());
		this->iCount += this->iBufferSize;

	} // switch transport type

} // timerEvent



}	} // namespace SwissalpS::QtSssSapp
