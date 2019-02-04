#ifndef SwissalpS_QtSssSapp_WSSCLIENT_H
#define SwissalpS_QtSssSapp_WSSCLIENT_H

#include <QObject>
#include <QWebSocket>



namespace SwissalpS { namespace QtSssSapp {



class WSSclient : public QObject {

	Q_OBJECT

private:

private slots:
	void onConnected();
	void onDisconnected();

protected:
	bool bConnected;
	QUrl oURL;
	QWebSocket *pWebSocket;

public:
	explicit WSSclient(const QUrl oURL, QObject *pParent = nullptr);
	virtual ~WSSclient();

	inline virtual bool isConnected() const { return this->bConnected; }

signals:
	void debugMessage(const QString &sMessage) const;

public slots:
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("WSSclient:" + sMessage); }

	virtual void onBinaryMessageReceived(const QByteArray &aMessage);
	virtual void onTextMessageReceived(const QString &sMessage);

}; // WSSclient



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_WSSCLIENT_H
