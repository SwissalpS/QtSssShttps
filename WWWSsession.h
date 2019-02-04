#ifndef SwissalpS_QtSssSapp_WWWSSESSION_H
#define SwissalpS_QtSssSapp_WWWSSESSION_H

#include <QObject>
#include <QSslSocket>
#include <QNetworkProxy>



namespace SwissalpS { namespace QtSssSapp {



class WWWSrequest;



class WWWSsession : public QObject {

	Q_OBJECT

private:

protected:
	QSslSocket *pSocket;
	WWWSrequest *pRequest;

public:
	explicit WWWSsession(QSslSocket *pSocket, QObject *pParent = nullptr);
	virtual ~WWWSsession();

	static QString currentRFC7231date();
	virtual inline QSslSocket *socket() const { return this->pSocket; }
	virtual void start();

signals:
	void debugMessage(const QString &sMessage) const;
	void disconnected(WWWSsession *pSession) const;
	void gotRequest(WWWSrequest *pRequest) const;

public slots:
	virtual void onChannelReadyRead(const int iChannel);
	virtual void onConnected();

	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("WWWSsession:" + sMessage); }

	virtual inline void onDisconnected() { Q_EMIT this->disconnected(this); }
	virtual void onEncrypted();
	virtual void onEncryptedBytesWritten(const qint64 illCount);
	virtual void onError(const QAbstractSocket::SocketError eError);
	virtual void onModeChanged(const QSslSocket::SslMode eMode);
	virtual void onPeerVerifyError(const QSslError oError);
	virtual void onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator *pPSKauthenticator);
	virtual void onProxyAuthenticationRequired(QNetworkProxy oProxy, QAuthenticator *pAuthenticator);
	virtual void onReadChannelFinished();
	virtual void onReadyRead();
	virtual void onSSLerrors(const QList<QSslError> aoErrors);
	virtual void onStateChanged(const QAbstractSocket::SocketState eState);

}; // WWWSsession



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_WWWSSESSION_H
