#ifndef SwissalpS_QtSssSapp_WWWSSESSION_H
#define SwissalpS_QtSssSapp_WWWSSESSION_H

#include <QObject>
#include <QSslSocket>
#include <QNetworkProxy>



namespace SwissalpS { namespace QtSssSapp {



class WWWSrequest;


// is owned by WWWSserver
// owns WWWSrequest and deletes QSslSocket
class WWWSsession : public QObject {

	Q_OBJECT

private:

protected:
	QSslSocket *pSocket;
	WWWSrequest *pRequest;
	uint uiMaxRequestSize;

public:
	quint64 uSessionID;
	explicit WWWSsession(QSslSocket *pSocket, QObject *pParent = nullptr);
	virtual ~WWWSsession();

	inline virtual uint maxRequestSize() const { return this->uiMaxRequestSize; }
	inline virtual void setMaxRequestSize(const uint uiNewSize) {
		this->uiMaxRequestSize = uiNewSize; }

	virtual inline QSslSocket *socket() const { return this->pSocket; }
	// start the connection process
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
