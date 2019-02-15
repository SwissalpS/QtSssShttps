#ifndef SwissalpS_QtSssSapp_MHTTPSSHANDLERREDIRECT_H
#define SwissalpS_QtSssSapp_MHTTPSSHANDLERREDIRECT_H

#include <QObject>
#include <QRegularExpression>

#include "MHTTPSShandlerBase.h"


namespace SwissalpS { namespace QtSssSapp {



class MHTTPSShandlerRedirect : public MHTTPSShandlerBase {

	Q_OBJECT

private:

protected:
	QRegularExpression oREsearch;
	QString sREreplace;

public:
	explicit MHTTPSShandlerRedirect(QJsonObject ojoHandlerConf,
									QObject *pParent = nullptr) :
		MHTTPSShandlerBase(ojoHandlerConf, pParent) {}

	inline virtual ~MHTTPSShandlerRedirect() {}

	virtual void init() override;

signals:

public slots:
	virtual void handle(WWWSrequest *pRequest) override;
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("MHTTPSShandlerRedirect:" + sMessage); }

}; // MHTTPSShandlerRedirect



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_MHTTPSSHANDLERREDIRECT_H
