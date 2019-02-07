#ifndef SwissalpS_QtSssSapp_MODULEAPPCONTROL_H
#define SwissalpS_QtSssSapp_MODULEAPPCONTROL_H

#include <QObject>

#include "AppSettings.h"
#include "ModuleBase.h"
#include "WSSserver.h"
#include "WSSsession.h"
#include "WWWSrequest.h"
#include "WWWSserver.h"



namespace SwissalpS { namespace QtSssSapp {



class AppController;


class ModuleAppControl : public ModuleBase {

	Q_OBJECT

private:

protected:
	AppController *pAC;
	AppSettings *pAS;
	WSSserver *pWebSocketServer;
	WWWSserver *pWWWSserver;

	void handleLogin(QJsonObject oJarguments, WSSsession *pSession);
	void initAuthentication() override;
	void initSSL() override;
	void initWebSocketServer();
	void initWWWserver();
	void startWebSocketServer();
	void startWWWserver();
	void stopWebSocketServer();
	void stopWWWserver();

public:
	ModuleAppControl(ModuleConf *pConf, QObject *pParent = nullptr);
	virtual ~ModuleAppControl();

	void die() override;
	bool executeBusCommand(const QString &sCommand, const QJsonObject &ojoMessage) override;
	QStringList exposedBusCommands() const override;
	void init() override;
	void start() override;
	void stop() override;

signals:

public slots:
	inline void onDebugMessage(const QString &sMessage) const override {
		Q_EMIT this->debugMessage("ModAC:" + sMessage); }

	void onGotJSON(const QString &sCommand, QJsonObject oJarguments,
				   WSSsession *pSession);
	void onWWWSrequest(WWWSrequest *pRequest);


}; // ModuleAppControl



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_MODULEAPPCONTROL_H
