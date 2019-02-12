#ifndef SwissalpS_QtSssSapp_MHTTPSSHANDLERDROP_H
#define SwissalpS_QtSssSapp_MHTTPSSHANDLERDROP_H

#include <QObject>

#include "MHTTPSShandlerRedirect.h"



namespace SwissalpS { namespace QtSssSapp {



class MHTTPSShandlerDrop : public MHTTPSShandlerRedirect {

	Q_OBJECT

public:
	explicit MHTTPSShandlerDrop(QJsonObject ojoHandlerConf,
								QObject *pParent = nullptr) :
		MHTTPSShandlerRedirect(ojoHandlerConf, pParent) {}

	virtual ~MHTTPSShandlerDrop() {}

signals:
	void debugMessage(const QString &sMessage) const;

public slots:
	virtual void handle(WWWSrequest *pRequest) override;
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("MHTTPSShandlerDrop:" + sMessage); }

}; // MHTTPSShandlerDrop



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_MHTTPSSHANDLERDROP_H
