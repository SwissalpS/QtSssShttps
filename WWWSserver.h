#ifndef SwissalpS_QtSssSapp_WWWSSERVER_H
#define SwissalpS_QtSssSapp_WWWSSERVER_H

#include <QObject>
#include <QList>
#include <QSslKey>
#include <QtNetwork/QSslError>
#include <QtNetwork/QHostAddress>
#include <QTcpServer>
#include <QVector>
#include <QWebSocketCorsAuthenticator>
#include <QWebSocketProtocol>

#include "WWWSrequest.h"
#include "WWWSsession.h"



namespace SwissalpS { namespace QtSssSapp {



class WWWSresponse;



class WWWSserver : public QTcpServer {

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
	QTcpServer *pTCPserver;
	QSslCertificate oCertificate;
	QHostAddress oHost;
	QSslKey oPrivateKey;
	QString sServerString;
	quint16 uiMaxConnections;
	quint16 uiPort;
	QVector<WWWSsession *> apSessions;

	void incomingConnection(qintptr ipSocketDescriptor) override;

public:
	explicit WWWSserver(QObject *pParent = nullptr);
	virtual ~WWWSserver();

	static QString currentRFC7231date();
	inline virtual QHostAddress host() const { return this->oHost; }
	virtual void init(const QHostAddress oHost, const quint16 uiPort,
					  const QSslCertificate &oCert, const QSslKey &oKey);

	virtual bool isBlockedIP(const QString &sIP);
	inline virtual bool isListBlack() const { return this->bListIsBlack; }

	inline virtual bool isListening() const {
		return this->pTCPserver ? this->pTCPserver->isListening() : false; }

	inline virtual QStringList listBW() const { return this->asListBW; }
	inline virtual quint16 port() const { return this->uiPort; }
	// TODO: refactor all* respond methods to create a WWWSresponse
	// which this method merely fills up empty headers and then starts
	// a chunked write that does not interfere with event-loop.
	// That way any subclass of WWWSresponse can be sent here.
	// also creating dirlist may need calls to event-loop.
	// *simple header only responses such as 301 but also simple 404
	virtual void respond(WWWSresponse *pResponse);
	virtual void respond(WWWSrequest *pRequest, QString &sBody,
						 const quint16 uiCode = 200u);
	virtual void respond200(WWWSrequest *pRequest, const QString &sBody,
					  const QString &sContentHeaderValue = "text/html");
	virtual void respond404(WWWSrequest *pRequest);
	virtual void respond301(WWWSrequest *pRequest, const QString &sURL);
	virtual void respondDirList(WWWSrequest *pRequest, const QString &sPath);
	virtual void respondFile(WWWSrequest *pRequest, const QString &sPathFile);
	virtual void respondJSON(WWWSrequest *pRequest, const QString &sJSON);
	inline virtual void setListBW(QStringList asList) { this->asListBW = asList; }
	inline virtual void setListIsBlack(const bool bListIsBlackNotWhite) {
		this->bListIsBlack = bListIsBlackNotWhite; }

	inline virtual void setMaxConnections(const int iMax) {
		this->uiMaxConnections = quint16(iMax);
		if (!this->pTCPserver) return;
		this->pTCPserver->setMaxPendingConnections(iMax); }

	virtual inline QString serverString() const { return this->serverString(); }
	virtual inline void setServerString(const QString &sNewSS) {
		this->sServerString = sNewSS; }

	virtual void start();
	virtual void stop();

signals:
	void debugMessage(const QString &sMessage) const;
	void gotRequest(WWWSrequest *pRequest) const;

public slots:
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("WWWS:" + sMessage); }

	virtual void onDisconnected(WWWSsession *pSession);
	inline void onGotRequest(WWWSrequest *pRequest) const {
		Q_EMIT this->gotRequest(pRequest); }


}; // WWWSserver



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_WWWSSERVER_H
