#ifndef SwissalpS_QtSssSapp_MODULEHTTPSSERVER_H
#define SwissalpS_QtSssSapp_MODULEHTTPSSERVER_H

#include <QObject>

#include "ModuleBase.h"
#include "WWWSserver.h"



namespace SwissalpS { namespace QtSssSapp {



class MHTTPSShandlerDelegateBase : public QObject {

	Q_OBJECT

private:

protected:

public:
	explicit MHTTPSShandlerDelegateBase(QObject *pParent = nullptr) :
		QObject(pParent) {}

	virtual ~MHTTPSShandlerDelegateBase() {}

	// blocking method!
	virtual bool handle(WWWSrequest *pRequest) {
		Q_UNUSED(pRequest)
		return false; }

signals:
	void debugMessage(const QString &sMessage) const;

public slots:
	inline virtual void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("MHTTPSShandlerDelegateBase:" + sMessage); }

}; // MHTTPSShandlerDelegateBase



class MHTTPSShandlerBase : public QObject {

	Q_OBJECT

protected:
	MHTTPSShandlerDelegateBase *pDelegate;
	QString sPathBase;

protected slots:

public:
	explicit MHTTPSShandlerBase(const QString sPathBase,
								QObject *pParent = nullptr) :
		QObject(pParent),
		pDelegate(nullptr),
		sPathBase(sPathBase) {}

	virtual ~MHTTPSShandlerBase() {}

	inline virtual QString pathBase() const { return this->sPathBase; }
	inline virtual void setDelegate(MHTTPSShandlerDelegateBase *pDelegate) {
		if (this->pDelegate) disconnect(this->pDelegate);
		this->pDelegate = pDelegate;
		connect(this->pDelegate, SIGNAL(debugMessage(QString)),
				this, SLOT(onDebugMessage(QString))); }

	inline virtual void setPathBase(const QString &sPathNew) {
		this->sPathBase = sPathNew.endsWith('/') ? sPathNew
												 : "" + sPathNew + "/"; }

signals:
	void nextHandler(WWWSrequest *pRequest) const;
	void debugMessage(const QString &sMessage) const;

public slots:
	// not sure this needs to be a slot. It could be as we can mark the request as handled.
	inline virtual void handle(WWWSrequest *pRequest) {
		if (nullptr != this->pDelegate) {
			if (!this->pDelegate->handle(pRequest))
				Q_EMIT this->nextHandler(pRequest); }
		else Q_EMIT this->nextHandler(pRequest); }

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



class MHTTPSShandlerRedirect : public MHTTPSShandlerBase {

	Q_OBJECT

public:
	explicit MHTTPSShandlerRedirect(const QString sPathBase,
									QObject *pParent = nullptr) :
		MHTTPSShandlerBase(sPathBase, pParent) {}

	virtual ~MHTTPSShandlerRedirect() {}

signals:
	void debugMessage(const QString &sMessage) const;

public slots:
	inline virtual void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("MHTTPSShandlerRedirect:" + sMessage); }

}; // MHTTPSShandlerRedirect



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

	void onNextHandler(WWWSrequest *pRequest);
	void onRequest(WWWSrequest *pRequest);

}; // ModuleHTTPSserver



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_MODULEHTTPSSERVER_H
