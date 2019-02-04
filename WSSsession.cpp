#include "WSSsession.h"

#include <QJsonDocument>
#include <QJsonObject>



namespace SwissalpS { namespace QtSssSapp {



WSSsession::WSSsession(QWebSocket *pSocket, QObject *pParent) :
	QObject(pParent),
	pWebSocket(pSocket) {

	if (0 == pSocket) {

		qFatal("KO::Passed null QWebSocket to WSSsession constructor!");

		return;

	} // if null passed

	connect(this->pWebSocket, SIGNAL(disconnected()),
			this, SLOT(onDisconnected()));

	connect(this->pWebSocket, SIGNAL(binaryMessageReceived(QByteArray)),
			this, SLOT(onBinaryMessageReceived(QByteArray)));

	connect(this->pWebSocket, SIGNAL(textMessageReceived(QString)),
			this, SLOT(onTextMessageReceived(QString)));

} // construct


WSSsession::~WSSsession() {

	if (this->pWebSocket) this->pWebSocket->deleteLater();

} // dealloc


void WSSsession::onBinaryMessageReceived(const QByteArray &aMessage) {

	this->onDebugMessage("OK:got binary data:0x" + QString(aMessage.toHex()));

} // onBinaryMessageReceived


void WSSsession::onTextMessageReceived(const QString &sMessage) {

	this->onDebugMessage("OK:got Text:" + sMessage);

	QJsonDocument oJdoc = QJsonDocument::fromJson(sMessage.toUtf8());
	if (oJdoc.isNull()) return;
	if (!oJdoc.isObject()) return;

	QJsonObject oJo = oJdoc.object();
	if (oJo.isEmpty()) return;

	QString sCommand = oJo.value("command").toString("");
	if (sCommand.isEmpty()) return;

	this->onDebugMessage("command: " + sCommand);

	Q_EMIT this->gotJSON(sCommand, oJo.value("arguments").toObject(), this);

} // onTextMessageReceived



}	} // namespace SwissalpS::QtSssSapp

