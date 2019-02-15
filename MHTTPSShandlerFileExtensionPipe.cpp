#include "MHTTPSShandlerFileExtensionPipe.h"

#include <QDir>
#include <QFileInfo>



namespace SwissalpS { namespace QtSssSapp {



void MHTTPSShandlerFileExtensionPipe::doMatch(WWWSrequest *pRequest) {

	this->onDebugMessage("doMatch");

	MHTTPSShFEPexe *pExe = new MHTTPSShFEPexe(pRequest, this->sPathWaker,
											  QStringList() << pRequest->path());

	connect(pExe, SIGNAL(debugMessage(QString)),
			this, SLOT(onDebugMessage(QString)));

	connect(pExe, SIGNAL(respond(WWWSresponse*)),
			this, SLOT(onRespond(WWWSresponse*)));

	pExe->start();

	// check if pipe exists

	// if not wake

	// check again

	// etc, so just start a QProcess controller...which reports back here...

} // doMatch


void MHTTPSShandlerFileExtensionPipe::init() {

	MHTTPSShandlerFileExtensionBase::init();

	if (!this->bActive) return;

	this->sPathPipe = this->ojoConfig.value(
						  ModuleConf::sTagMHTTPSSHfileSocketPath).toString();

	this->sPathWaker = this->ojoConfig.value(
						  ModuleConf::sTagMHTTPSSHfileSocketStarter).toString();

} // init



MHTTPSShFEPexe::MHTTPSShFEPexe(WWWSrequest *pRequest, const QString &sShellScript,
							   QStringList &asArguments) :
	QObject(pRequest),
	bHaveReadHeaders(false),
	pRequest(pRequest),
	pResponse(new WWWSresponse(pRequest)),
	pProcess(nullptr) {

	this->pProcess = new QProcess(this);
	this->pProcess->setProgram(sShellScript);
	this->pProcess->setArguments(asArguments);
	QProcessEnvironment oPE = QProcessEnvironment::systemEnvironment();
	// https://tools.ietf.org/html/rfc3875
	// this did not seem to work. We may need to launch /bin/sh and then
	// only start php-cgi
	oPE.insert("QUERY_STRING", this->pRequest->query());
	this->pProcess->setProcessEnvironment(oPE);

	connect(this->pProcess, SIGNAL(error(QProcess::ProcessError)),
			this, SLOT(onError(QProcess::ProcessError)));

	connect(this->pProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
			this, SLOT(onFinished(int,QProcess::ExitStatus)));

	connect(this->pProcess, SIGNAL(readyReadStandardError()),
			this, SLOT(onReadyReadStandardError()));

	connect(this->pProcess, SIGNAL(readyReadStandardOutput()),
			this, SLOT(onReadyReadStandardOutput()));

	connect(this->pProcess, SIGNAL(started()),
			this, SLOT(onStarted()));

} // construct


void MHTTPSShFEPexe::start() {

	this->onDebugMessage("start");

	this->pProcess->start();

} // start


void MHTTPSShFEPexe::onError(QProcess::ProcessError eError) {

	QString sMessage = "onError:";

	switch (eError) {

		case QProcess::FailedToStart: sMessage += "file not found, resource error"; break;
		case QProcess::Crashed: sMessage += "Crashed"; break;
		case QProcess::Timedout: sMessage += "Timedout"; break;
		case QProcess::ReadError: sMessage += "ReadError"; break;
		case QProcess::WriteError: sMessage += "WriteError"; break;
		default:
		case QProcess::UnknownError: sMessage += "UnknownError"; break;

	} // switch eError

	this->onDebugMessage(sMessage);

} // onError


void MHTTPSShFEPexe::onFinished(int iExitCode, QProcess::ExitStatus eExitStatus) {

	this->onDebugMessage("onFinished " + QString::number(iExitCode)
						 + " " + QString::number(eExitStatus));

	this->pProcess->close();
	this->pProcess->deleteLater();

	Q_EMIT this->respond(this->pResponse);

} // onFinished


void MHTTPSShFEPexe::onReadyReadStandardError() {

	QString sOut(this->pProcess->readAllStandardError());

	this->onDebugMessage("onReadyReadStandardError: " + sOut);

	this->pResponse->setStatusCode(500);

} // onReadyReadStandardError


void MHTTPSShFEPexe::onReadyReadStandardOutput() {

	QString sOut(this->pProcess->readAllStandardOutput());

	this->onDebugMessage("onReadyReadStandardOutput");

	if (this->bHaveReadHeaders) {

		this->pResponse->setBody(this->pResponse->body() + sOut);
		return;

	} // if already have headers

	QStringList asLines = sOut.split("\n");
	int iCountHeaderlines;
	QString sLine;
	for (iCountHeaderlines = 0; iCountHeaderlines < asLines.length();
		 ++iCountHeaderlines) {

		sLine = asLines.at(iCountHeaderlines).trimmed();

		if (sLine.isEmpty()) break;

		pResponse->addHeader(new WWWheader(sLine.toUtf8()));

	} // loop

	iCountHeaderlines++;
	if (asLines.length() > iCountHeaderlines) {

		pResponse->setBody(asLines.mid(iCountHeaderlines).join("\n"));

	} else {

		pResponse->setStatusCode(204);

	} // if have enough lines (this can happen if output is empty)

} // onReadyReadStandardOutput


void MHTTPSShFEPexe::onStarted() {

	this->onDebugMessage("onStarted");
	// this does not seem to be the way to do it but we could
	// pass on more or less all processing to called script
	this->pProcess->write(this->pRequest->rawRequest());

} // onStarted



void MHTTPSShandlerFileExtensionLocalSocket::doMatch(WWWSrequest *pRequest) {

	this->onDebugMessage("doMatch");

	MHTTPSShFELSexe *pExe = new MHTTPSShFELSexe(pRequest, this->sPathSocket);

	connect(pExe, SIGNAL(debugMessage(QString)),
			this, SLOT(onDebugMessage(QString)));

//	connect(pExe, SIGNAL(respond(MHTTPSShFELSexe*)),
//			this, SLOT(onRespondFromExe(MHTTPSShFELSexe*)));

	pExe->start();

} // doMatch


void MHTTPSShandlerFileExtensionLocalSocket::init() {

	MHTTPSShandlerFileExtensionBase::init();

	if (!this->bActive) return;

	this->sPathSocket = this->ojoConfig.value(
						  ModuleConf::sTagMHTTPSSHfileSocketPath).toString();

	this->sPathWaker = this->ojoConfig.value(
						  ModuleConf::sTagMHTTPSSHfileSocketStarter).toString();

} // init



MHTTPSShFELSexe::MHTTPSShFELSexe(WWWSrequest *pRequest, const QString &sPathSocket) :
	QObject(pRequest),
	pRequest(pRequest),
	pSocket(nullptr) {

	this->pSocket = new QLocalSocket(this);
	this->pSocket->setServerName(sPathSocket);

	connect(this->pSocket, SIGNAL(connected()),
			this, SLOT(onConnected()));

	connect(this->pSocket, SIGNAL(disconnected()),
			this, SLOT(onDisconnected()));

	connect(this->pSocket, SIGNAL(error(QLocalSocket::LocalSocketError)),
			this, SLOT(onError(QLocalSocket::LocalSocketError)));

	connect(this->pSocket, SIGNAL(readyRead()),
			this, SLOT(onReadyRead()));

	//this->pSocket->connectToServer();

} // construct


void MHTTPSShFELSexe::onError(QLocalSocket::LocalSocketError eError) {

	this->onDebugMessage("onError");

	QString sMessage;

	switch (eError) {

		case QAbstractSocket::ConnectionRefusedError: sMessage = "ConnectionRefusedError"; break;
		case QAbstractSocket::RemoteHostClosedError: sMessage = "PeerClosedError"; break;
		case QAbstractSocket::HostNotFoundError: sMessage = "ServerNotFoundError"; break;
		case QAbstractSocket::SocketAccessError: sMessage = "SocketAccessError"; break;
		case QAbstractSocket::SocketResourceError: sMessage = "SocketResourceError"; break;
		case QAbstractSocket::SocketTimeoutError: sMessage = "SocketTimeoutError"; break;
		case QAbstractSocket::DatagramTooLargeError: sMessage = "DatagramTooLargeError"; break;
		case QAbstractSocket::NetworkError: sMessage = "ConnectionError"; break;
		case QAbstractSocket::UnsupportedSocketOperationError: sMessage = "UnsupportedSocketOperationError"; break;
		case QAbstractSocket::UnknownSocketError: sMessage = "UnknownSocketError"; break;
		case QAbstractSocket::OperationError: sMessage = "OperationError"; break;

		default:
			sMessage = "Unknown error: " + QString::number(eError);
		break;

	} // switch eError

	this->onDebugMessage(sMessage);

}


void MHTTPSShFELSexe::onConnected() {

	this->onDebugMessage("onConnected");
	qint64 iCount = this->pSocket->write(this->pRequest->rawRequest());//"<?php echo \"hahah\"; ?>"); //
	this->pSocket->flush();

	if (-1 == iCount) {
		this->onDebugMessage("error, could not write");
	}

} // onConnected


void MHTTPSShFELSexe::onDisconnected() {

	this->onDebugMessage("onDisconnected");

} // onDisconnected


void MHTTPSShFELSexe::onReadyRead() {

	this->onDebugMessage("onReadyRead");

	this->onDebugMessage(QString(this->pSocket->readAll()));

} // onReadyRead



}	} // namespace SwissalpS::QtSssSapp
