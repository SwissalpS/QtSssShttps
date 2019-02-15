#ifndef SwissalpS_QtSssSapp_MHTTPSSHANDLERFILEEXTENSIONBASE_H
#define SwissalpS_QtSssSapp_MHTTPSSHANDLERFILEEXTENSIONBASE_H

#include <QObject>

#include "MHTTPSShandlerRedirect.h"



namespace SwissalpS { namespace QtSssSapp {



class MHTTPSShandlerFileExtensionBase : public MHTTPSShandlerRedirect {

	Q_OBJECT

private:

protected:
	QString sExtension;

public:
	explicit MHTTPSShandlerFileExtensionBase(QJsonObject ojoHandlerConf,
									  QObject *pParent = nullptr) :
		MHTTPSShandlerRedirect(ojoHandlerConf, pParent),
		sExtension("") {}

	inline virtual ~MHTTPSShandlerFileExtensionBase() {}

	virtual void init() override;

signals:

public slots:
	virtual void handle(WWWSrequest *pRequest) override;
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("MHTTPSShandlerFileExtensionBase:" + sMessage); }

}; // MHTTPSShandlerFileExtensionBase



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_MHTTPSSHANDLERFILEEXTENSIONBASE_H
