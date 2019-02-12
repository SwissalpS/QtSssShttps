#ifndef SwissalpS_QtSssSapp_MHTTPSSHANDLERDELEGATEBASE_H
#define SwissalpS_QtSssSapp_MHTTPSSHANDLERDELEGATEBASE_H

#include <QObject>

#include "WWWSrequest.h"



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



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_MHTTPSSHANDLERDELEGATEBASE_H
