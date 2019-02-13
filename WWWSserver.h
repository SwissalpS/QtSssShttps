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
#include "WWWSresponse.h"
#include "WWWSsession.h"



namespace SwissalpS { namespace QtSssSapp {



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
	QStringList asListBasePaths;
	QStringList asListBW;
	QTcpServer *pTCPserver;
	QSslCertificate oCertificate;
	QHostAddress oHost;
	QSslKey oPrivateKey;
	QString sServerString;
	quint64 uSessionCounter;
	quint16 uiMaxConnections;
	uint uiMaxRequestSize;
	quint16 uiPort;
	QVector<WWWSsession *> apSessions;

	void incomingConnection(qintptr ipSocketDescriptor) override;

public:
	explicit WWWSserver(QObject *pParent = nullptr);
	virtual ~WWWSserver();

	inline virtual QStringList basePaths() const { return this->asListBasePaths; }
	static QString currentRFC7231date();
	inline virtual QHostAddress host() const { return this->oHost; }
	virtual void ignore(WWWSrequest *pRequest);
	virtual void init(const QHostAddress oHost, const quint16 uiPort,
					  const QSslCertificate &oCert, const QSslKey &oKey);

	virtual bool isBlockedIP(const QString &sIP);
	inline virtual bool isListBlack() const { return this->bListIsBlack; }

	inline virtual bool isListening() const {
		return this->pTCPserver ? this->pTCPserver->isListening() : false; }

	virtual bool isValidPath(const QString &sPath);
	inline virtual QStringList listBW() const { return this->asListBW; }
	inline virtual uint maxRequestSize() const { return this->uiMaxRequestSize; }
	inline virtual quint16 port() const { return this->uiPort; }
	// all the respond*() methods bellow demonstrate how to call this
	virtual void respond(WWWSresponse *pResponse);
	// convenience methods and demos on how to use respond(WWWSresponse*)
	virtual void respond(WWWSrequest *pRequest, const QString &sBody,
						 const quint16 uiCode = 200u);
	virtual void respond200(WWWSrequest *pRequest, const QString &sBody,
					  const QString &sContentHeaderValue = "text/html");
	virtual void respond404(WWWSrequest *pRequest);
	virtual void respond301(WWWSrequest *pRequest, const QString &sURL);
	// TODO: creating dirlist may need calls to event-loop.
	// for now we just limit to 2000 entries
	virtual void respondDirectoryList(WWWSrequest *pRequest, const QString &sPath);
	virtual void respondFile(WWWSrequest *pRequest, const QString &sPathFile);
	virtual void respondJSON(WWWSrequest *pRequest, const QString &sJSON);
	// only serve resources with these base-paths (assumes absolute paths)
	inline virtual void setBasePaths(QStringList asPaths) {
		this->asListBasePaths = asPaths; }

	// set the list of IPs that you want to block(black) or allow(white)
	inline virtual void setListBW(QStringList asList) { this->asListBW = asList; }
	// set the colour of the IP-list
	inline virtual void setListIsBlack(const bool bListIsBlackNotWhite) {
		this->bListIsBlack = bListIsBlackNotWhite; }

	inline virtual void setMaxConnections(const int iMax) {
		this->uiMaxConnections = quint16(iMax);
		if (!this->pTCPserver) return;
		this->pTCPserver->setMaxPendingConnections(iMax); }

	inline virtual void setMaxRequestSize(const uint uiNewSize) {
		this->uiMaxRequestSize = uiNewSize; }

	// As what to identify this server in headers
	virtual inline QString serverString() const { return this->serverString(); }
	virtual inline void setServerString(const QString &sNewSS) {
		this->sServerString = sNewSS; }

	// start/stop listening
	virtual void start();
	virtual void stop();

signals:
	void debugMessage(const QString &sMessage) const;
	// relay from WWWSsession
	void gotRequest(WWWSrequest *pRequest) const;

public slots:
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("WWWS:" + sMessage); }

	virtual void onDisconnected(WWWSsession *pSession);
	// relay from WWWSsession
	inline void onGotRequest(WWWSrequest *pRequest) const {
		Q_EMIT this->gotRequest(pRequest); }


}; // WWWSserver



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_WWWSSERVER_H
