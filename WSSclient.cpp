#include "WSSclient.h"



namespace SwissalpS { namespace QtSssSapp {



WSSclient::WSSclient(const QUrl oURL, QObject *pParent) :
	QObject(pParent),
	bConnected(false),
	oURL(oURL),
	pWebSocket(nullptr) {

	this->pWebSocket = new QWebSocket("", QWebSocketProtocol::VersionLatest, this);

	connect(this->pWebSocket, SIGNAL(connected()),
			this, SLOT(onConnected()));

	connect(this->pWebSocket, SIGNAL(disconnected()),
			this, SLOT(onDisconnected()));

	this->pWebSocket->open(this->oURL);

} // construct


WSSclient::~WSSclient() {

	this->bConnected = false;

	delete this->pWebSocket;
	this->pWebSocket = nullptr;

} // dealloc


void WSSclient::onBinaryMessageReceived(const QByteArray &aMessage) {

	this->onDebugMessage("onBinaryMessageReceived: " + QString(aMessage));

	this->pWebSocket->close();

} // onBinaryMessageReceived


void WSSclient::onConnected() {

	this->onDebugMessage("onNewConnection");

	connect(this->pWebSocket, SIGNAL(binaryMessageReceived(QByteArray)),
			this, SLOT(onBinaryMessageReceived(QByteArray)));

	connect(this->pWebSocket, SIGNAL(textMessageReceived(QString)),
			this, SLOT(onTextMessageReceived(QString)));

	//this->pWebSocket->ping();
	this->pWebSocket->sendTextMessage(QStringLiteral("PING"));
	//this->pWebSocket->sendBinaryMessage(QByteArray() << "P");

} // onConnected


void WSSclient::onDisconnected() {

	this->onDebugMessage("onDisconnected");

	this->bConnected = false;

	delete this->pWebSocket;
	this->pWebSocket = nullptr;

} // onDisconnected


void WSSclient::onTextMessageReceived(const QString &sMessage) {

	this->onDebugMessage("onTextMessageReceived: " + sMessage);

	this->pWebSocket->close();

} // onTextMessageReceived



}	} // namespace SwissalpS::QtSssSapp

