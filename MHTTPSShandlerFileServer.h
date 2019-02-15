#ifndef SwissalpS_QtSssSapp_MHTTPSSHANDLERFILESERVER_H
#define SwissalpS_QtSssSapp_MHTTPSSHANDLERFILESERVER_H

#include <QObject>

#include "MHTTPSShandlerRedirect.h"



namespace SwissalpS { namespace QtSssSapp {



class MHTTPSShandlerFileServer : public MHTTPSShandlerRedirect {

	Q_OBJECT

private:

protected:
	bool bListDirs;
	QString sDirectoryIndex;

public:
	explicit MHTTPSShandlerFileServer(QJsonObject ojoHandlerConf,
									  QObject *pParent = nullptr) :
		MHTTPSShandlerRedirect(ojoHandlerConf, pParent),
		bListDirs(false),
		sDirectoryIndex("") {}

	inline virtual ~MHTTPSShandlerFileServer() {}

	virtual void init() override;

signals:

public slots:
	virtual void handle(WWWSrequest *pRequest) override;
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("MHTTPSShandlerFileServer:" + sMessage); }

}; // MHTTPSShandlerFileServer



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_MHTTPSSHANDLERFILESERVER_H
