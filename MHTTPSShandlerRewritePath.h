#ifndef SwissalpS_QtSssSapp_MHTTPSSHANDLERREWRITEPATH_H
#define SwissalpS_QtSssSapp_MHTTPSSHANDLERREWRITEPATH_H

#include <QObject>

#include "MHTTPSShandlerRedirect.h"



namespace SwissalpS { namespace QtSssSapp {



class MHTTPSShandlerRewritePath : public MHTTPSShandlerRedirect {

	Q_OBJECT

private:

protected:
	bool bMatchIsFinal;

public:
	explicit MHTTPSShandlerRewritePath(QJsonObject ojoHandlerConf,
								   QObject *pParent = nullptr) :
		MHTTPSShandlerRedirect(ojoHandlerConf, pParent) {}

	inline virtual ~MHTTPSShandlerRewritePath() {}

	virtual void init() override;

	inline virtual bool isRewriteHandlerClass() const override { return true; }

signals:
	void debugMessage(const QString &sMessage) const;

public slots:
	virtual void handle(WWWSrequest *pRequest) override;
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("MHTTPSShandlerRewritePath:" + sMessage); }

}; // MHTTPSShandlerRewritePath



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_MHTTPSSHANDLERREWRITEPATH_H
