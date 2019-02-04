#ifndef SwissalpS_QtSssSapp_WSSSESSION_H
#define SwissalpS_QtSssSapp_WSSSESSION_H

#include <QJsonObject>
#include <QObject>
#include <QtWebSockets/QWebSocket>


namespace SwissalpS { namespace QtSssSapp {



//class QWebSocket;



class WSSsession : public QObject {

	Q_OBJECT

private:

protected:
	QWebSocket *pWebSocket;

public:
	explicit WSSsession(QWebSocket *pSocket, QObject *pParent = nullptr);
	virtual ~WSSsession();

signals:
	void debugMessage(const QString &sMessage) const;
	void disconnected() const;
	void gotJSON(const QString &sCommand, QJsonObject oJarguments,
				 WSSsession *pSession) const;

public slots:
	virtual void onBinaryMessageReceived(const QByteArray &aMessage);

	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("WSSsession:" + sMessage); }

	virtual inline void onDisconnected() const { Q_EMIT this->disconnected(); }
	virtual void onTextMessageReceived(const QString &sMessage);
	virtual inline QWebSocket *socket() const { return this->pWebSocket; }

}; // WSSsession



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_WSSSESSION_H
