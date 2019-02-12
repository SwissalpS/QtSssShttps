#ifndef SwissalpS_QtSssSapp_MHTTPSSHANDLERBASE_H
#define SwissalpS_QtSssSapp_MHTTPSSHANDLERBASE_H

#include <QObject>

#include "MHTTPSShandlerDelegateBase.h"
#include "ModuleConf.h"
#include "WWWSrequest.h"
#include "WWWSresponse.h"



namespace SwissalpS { namespace QtSssSapp {



class MHTTPSShandlerBase : public QObject {

	Q_OBJECT

private:

protected:
	MHTTPSShandlerDelegateBase *pDelegate;
	QJsonObject ojoConfig;
	QString sPathBase;
	QStringList asMethods;
	bool bActive;

	void initMethods();

public:
	explicit MHTTPSShandlerBase(QJsonObject ojoHandlerConf,
								QObject *pParent = nullptr);

	inline virtual ~MHTTPSShandlerBase() {}

	static QStringList allGenerllyUsedHTTPmethods();

	inline QString className() const {
		return this->ojoConfig.value(ModuleConf::sTagClass).toString(); }

	virtual void init();

	inline virtual bool isActive() const { return this->bActive; }
	inline virtual bool isRewriteHandlerClass() const { return false; }
	inline virtual QStringList methods() const { return this->asMethods; }
	inline virtual QString pathBase() const { return this->sPathBase; }
	virtual void setDelegate(MHTTPSShandlerDelegateBase *pDelegate);

	inline virtual void setPathBase(const QString &sPathNew) {
		this->sPathBase = sPathNew.endsWith('/') ? sPathNew
												 : "" + sPathNew + "/"; }

signals:
	void ignore(WWWSrequest *pRequest) const;
	void respond(WWWSresponse *pResponse) const;
	void respond(WWWSrequest *pRequest, const QString &sBody,
				 const quint16 uiCode = 200) const;
	void respond200(WWWSrequest *pRequest, const QString &sBody,
					const QString &sContentHeaderValue = "text/html") const;
	void respond404(WWWSrequest *pRequest) const;
	void respond301(WWWSrequest *pRequest, const QString &sURL) const;
	void respondDirectoryList(WWWSrequest *pRequest, const QString &sPath) const;
	void respondFile(WWWSrequest *pRequest, const QString &sPathFile) const;
	void respondJSON(WWWSrequest *pRequest, const QString &sJSON) const;
	void nextHandler(WWWSrequest *pRequest) const;
	void debugMessage(const QString &sMessage) const;

public slots:
	// not sure this needs to be a slot. It could be as we can mark the request as handled.
	virtual void handle(WWWSrequest *pRequest);
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("MHTTPSShandlerBase:" + sMessage); }

}; // MHTTPSShandlerBase



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_MHTTPSSHANDLERBASE_H
