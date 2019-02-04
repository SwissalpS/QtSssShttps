#ifndef SwissalpS_QtSssSapp_WSSSERVER_H
#define SwissalpS_QtSssSapp_WSSSERVER_H

#include <QJsonObject>
#include <QObject>
#include <QList>
#include <QtNetwork/QSslError>
#include <QtNetwork/QHostAddress>
#include <QtWebSockets/QWebSocketProtocol>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>



class QWebSocketServer;
class QWebSocket;



namespace SwissalpS { namespace QtSssSapp {



class WSSsession;



class WSSserver : public QObject {

	Q_OBJECT

private:

private slots:
	void onNewConnection();
	void onSocketDisconnected();
	void onSSLerrors(const QList<QSslError> &aErrors);

	void onOriginAuthenticationRequired(QWebSocketCorsAuthenticator *pWSCA);
	void onPeerVerifyError(QSslError oError);
	void onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator *pSSLPSKA);
	void onServerError(QWebSocketProtocol::CloseCode oCode);

protected:
	bool bListIsBlack;
	QStringList asListBW;
	QWebSocketServer *pWebSocketServer;
	QHostAddress oHost;
	quint16 uiMaxConnections;
	quint16 uiPort;
	QList<QWebSocket *> apClients;
	QList<WSSsession *> apSessions;

public:
	explicit WSSserver(QObject *pParent = nullptr);
	virtual ~WSSserver();

	inline virtual QHostAddress host() const { return this->oHost; }
	virtual void init(QHostAddress oHost, const quint16 uiPort,
					  const QSslCertificate &oCert, const QSslKey &oKey);
	virtual bool isBlockedIP(const QString &sIP);
	inline virtual bool isListBlack() const { return this->bListIsBlack; }

	inline virtual bool isListening() const {
		return this->pWebSocketServer ? this->pWebSocketServer->isListening() : false; }

	inline virtual QStringList listBW() const { return this->asListBW; }
	inline virtual quint16 port() const { return this->uiPort; }
	inline virtual void setListBW(QStringList asList) { this->asListBW = asList; }
	inline virtual void setListIsBlack(const bool bListIsBlackNotWhite) {
		this->bListIsBlack = bListIsBlackNotWhite; }
	inline virtual void setMaxConnections(const int iMax) {
		this->uiMaxConnections = quint16(iMax);
		if (!this->pWebSocketServer) return;
		this->pWebSocketServer->setMaxPendingConnections(iMax); }

	virtual void start();
	virtual void stop();

signals:
	void debugMessage(const QString &sMessage) const;
	void gotJSON(const QString &sCommand, QJsonObject oJarguments,
				 WSSsession *pSession) const;

public slots:
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("WSS:" + sMessage); }

	inline void onGotJSON(const QString &sCommand, QJsonObject oJarguments,
						  WSSsession *pSession) {
		Q_EMIT this->gotJSON(sCommand, oJarguments, pSession); }

}; // WSSserver



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_WSSSERVER_H
