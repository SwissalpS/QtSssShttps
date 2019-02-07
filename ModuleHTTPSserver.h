#ifndef SwissalpS_QtSssSapp_MODULEHTTPSSERVER_H
#define SwissalpS_QtSssSapp_MODULEHTTPSSERVER_H

#include <QObject>

#include "ModuleBase.h"
#include "WWWSserver.h"



namespace SwissalpS { namespace QtSssSapp {



class MHTTPSShandlerBase : public QObject {

	Q_OBJECT

protected:
	QString sPathBase;

public:
	explicit MHTTPSShandlerBase(const QString sPathBase, QObject *pParent = nullptr) {}
	virtual ~MHTTPSShandlerBase() {}

	virtual inline QString pathBase() const { return this->sPathBase; }
	virtual inline void setPathBase(const QString &sPathNew) {
		this->sPathBase = sPathNew.endsWith('/') ? sPathNew
												 : "" + sPathNew + "/"; }

signals:
	void debugMessage(const QString &sMessage) const;

public slots:
	// not sure this needs to be a slot. It could be as we can mark the request as handled.
	virtual bool handle(WWWSrequest *pRequest) {}
	inline virtual void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("MHTTPSShandlerBase:" + sMessage); }

}; // MHTTPSShandlerBase



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



// include dir listing options
//class MHTTPSShandlerFileServer : public MHTTPSShandlerBase {

//	Q_OBJECT

//public:
//	explicit MHTTPSShandlerFileServer(const QString sPathBase,
//									  QObject *pParent = nullptr) {}
//	virtual ~MHTTPSShandlerFileServer() {}

//signals:
//	void debugMessage(const QString &sMessage) const;

//public slots:
//	inline virtual void onDebugMessage(const QString &sMessage) const {
//		Q_EMIT this->debugMessage("MHTTPSShandlerFileServer:" + sMessage); }

//}; // MHTTPSShandlerFileServer



//class MHTTPSShandlerRedirect : public MHTTPSShandlerBase {

//	Q_OBJECT

//public:
//	explicit MHTTPSShandlerRedirect(const QString sPathBase,
//									QObject *pParent = nullptr) {}
//	virtual ~MHTTPSShandlerRedirect() {}

//signals:
//	void debugMessage(const QString &sMessage) const;

//public slots:
//	inline virtual void onDebugMessage(const QString &sMessage) const {
//		Q_EMIT this->debugMessage("MHTTPSShandlerRedirect:" + sMessage); }

//}; // MHTTPSShandlerRedirect



class ModuleHTTPSserver : public ModuleBase {

	Q_OBJECT
	Q_DISABLE_COPY(ModuleHTTPSserver)

private:

protected:

public:
	explicit ModuleHTTPSserver(ModuleConf *pConf, QObject *pParent = nullptr);
	virtual ~ModuleHTTPSserver();

signals:
	void debugMessage(const QString &sMessage) const;

public slots:
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("ModuleHTTPSserver:" + sMessage); }

}; // ModuleHTTPSserver



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_MODULEHTTPSSERVER_H
