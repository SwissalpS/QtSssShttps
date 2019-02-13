#ifndef SwissalpS_QtSssSapp_MODULEHTTPSSERVER_H
#define SwissalpS_QtSssSapp_MODULEHTTPSSERVER_H

#include <QObject>
#include <QRegularExpression>

#include "ModuleBase.h"
#include "MHTTPSShandlerBase.h"
#include "WWWSserver.h"



namespace SwissalpS { namespace QtSssSapp {



//class MHTTPSShandlerAuthentication : public MHTTPSShandlerBase {

//	Q_OBJECT

//public:
//	explicit MHTTPSShandlerAuthentication(QObject *pParent = nullptr) {}
//	virtual ~MHTTPSShandlerAuthentication() {}

//signals:
//	void debugMessage(const QString &sMessage) const;

//public slots:
//	inline virtual void onDebugMessage(const QString &sMessage) const {
//		Q_EMIT this->debugMessage("MHTTPSShandlerAuthentication:" + sMessage); }

//}; // MHTTPSShandlerAuthentication



//class MHTTPSShandlerCGI : public MHTTPSShandlerBase {

//	Q_OBJECT

//public:
//	explicit MHTTPSShandlerCGI(const QString sPathBase,
//							   QObject *pParent = nullptr) {}
//	virtual ~MHTTPSShandlerCGI() {}

//signals:
//	void debugMessage(const QString &sMessage) const;

//public slots:
//	inline virtual void onDebugMessage(const QString &sMessage) const {
//		Q_EMIT this->debugMessage("MHTTPSShandlerCGI:" + sMessage); }

//}; // MHTTPSShandlerCGI



class ModuleHTTPSserver : public ModuleBase {

	Q_OBJECT
	Q_DISABLE_COPY(ModuleHTTPSserver)

private:

protected:
	WWWSserver *pWWWSserver;
	QVector<MHTTPSShandlerBase *> apHandlers;

	void initHandlers();

public:
	explicit ModuleHTTPSserver(ModuleConf *pConf, QObject *pParent = nullptr);
	virtual ~ModuleHTTPSserver();

	static QHostAddress stringToHostAddress(const QString &sHostOrIP);
	void die() override;
	void init() override;
	inline WWWSserver *server() const { return this->pWWWSserver; }
	void start() override;
	void stop() override;

signals:
	void debugMessage(const QString &sMessage) const;

public slots:
	void onAppendHandler(QJsonObject ojoHandler);
	void onAppendHandler(MHTTPSShandlerBase *pHandler);
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("ModuleHTTPSserver:" + sMessage); }

	void onIgnore(WWWSrequest *pRequest);
	void onNextHandler(WWWSrequest *pRequest);
	void onRequest(WWWSrequest *pRequest);
	void onRespond(WWWSresponse *pResponse);
	void onRespond(WWWSrequest *pRequest, const QString &sBody,
				   const quint16 uiCode = 200);
	void onRespond200(WWWSrequest *pRequest, const QString &sBody,
					  const QString &sContentHeaderValue = "text/html");

	void onRespond404(WWWSrequest *pRequest);
	void onRespond301(WWWSrequest *pRequest, const QString &sURL);
	void onRespondDirectoryList(WWWSrequest *pRequest, const QString &sPath);
	void onRespondFile(WWWSrequest *pRequest, const QString &sPathFile);
	void onRespondJSON(WWWSrequest *pRequest, const QString &sJSON);

}; // ModuleHTTPSserver



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_MODULEHTTPSSERVER_H
