#ifndef SwissalpS_QtSssSapp_APPCONTROLLER_H
#define SwissalpS_QtSssSapp_APPCONTROLLER_H

#include <QHash>
#include <QObject>
#include <QTimer>

#include "AppSettings.h"
#include "ModuleBase.h"
#include "WSSserver.h"



namespace SwissalpS { namespace QtSssSapp {



class AppController : public QObject {

	Q_OBJECT

	static AppController *pSingelton;
	// keep this private as we want only one instance
	// read; http://www.qtcentre.org/wiki/index.php?title=Singleton_pattern
	explicit AppController(QObject *pParent = 0);
	// hide copy constructor
	explicit AppController(const AppController &other);
	// hide assign op
	// we leave just the declarations, so the compiler will warn us
	// if we try to use those two functions by accident
	AppController& operator=(const AppController &other);

private:

protected:
	AppSettings *pAS;
	QHash<QString, ModuleBase *> hpModules; // for quick access
	QStringList asModules; // for order
	QTimer *pTimerShutdown;
#ifdef SssS_USE_GUI
	bool bUseGUI;
#endif

	void checkDependencies();
#ifdef SssS_USE_GUI
	void initGUI();
#endif
	void initModules();
	void initRegistration();
	void initServer();
	void initSettings();
	void killModules();
	void loadModules();
	void writePID();

protected slots:
	virtual void timerEvent(QTimerEvent *pEvent) override;

public:
	~AppController();
	// destroy singelton
	static void drop();
	// public access to singelton instance
	static AppController *pAppController();

	void startModules();
	void stopModules();

signals:
	void debugMessageReceived(const QString &sMessage);
	void done();
	void modulesInitialized() const;
	void modulesStarted() const;
	void modulesStopped() const;
	void registerZeroConfServiceDescriptor(ZeroConfServiceDescriptor* pZCSD) const;
	void startZeroConfServices(const QString &sUID) const;
	void stopZeroConfServices(const QString &sUID) const;
	void unregisterZeroConfServiceDescriptors(const QString &sUID) const;

public slots:

	void onBusMessage(const QStringList &asUIDs, const QJsonObject &ojoMessage);
	void onDebugMessage(const QString &sMessage);
	inline void onRegisterZeroConfServiceDescriptor(ZeroConfServiceDescriptor* pZCSD) {
		Q_EMIT this->registerZeroConfServiceDescriptor(pZCSD); }

	inline void onServiceErrorNameCollision() {}
	inline void onServiceErrorRegistration() {}
	inline void onZeroConfServiceError() {}
	inline void onStartZeroConfServices(const QString &sUID) {
		Q_EMIT this->startZeroConfServices(sUID); }

	inline void onStopZeroConfServices(const QString &sUID) {
		Q_EMIT this->stopZeroConfServices(sUID); }

	inline void onUnregisterZeroConfServiceDescriptors(const QString &sUID) {
		Q_EMIT this->unregisterZeroConfServiceDescriptors(sUID); }

	void quit();
	void run();

}; // AppController



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_APPCONTROLLER_H
