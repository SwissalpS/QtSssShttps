#ifndef SwissalpS_QtSssSapp_MODULEBASE_H
#define SwissalpS_QtSssSapp_MODULEBASE_H

#include <QHostAddress>
#include <QJsonObject>
#include <QObject>
#include <QSslCertificate>
#include <QSslKey>

#include "ModuleConf.h"



namespace SwissalpS { namespace QtSssSapp {



class ModuleBase : public QObject {

	Q_OBJECT

private:

protected:
	ModuleConf *pMC;
	QSslCertificate oCertificate;
	QSslKey oPrivateKey;
	QStringList asListBW;

	// provide a base preparation that can easily be over-ridden
	// will not be called unless invoked (for now)
	virtual void initAuthentication();
	// provide a base preparation that can easily be over-ridden
	// will not be called unless invoked (for now)
	virtual void initSSL();

public:
	explicit ModuleBase(ModuleConf *pConf, QObject *pParent = nullptr);
	virtual ~ModuleBase();

	inline ModuleConf *config() const { return this->pMC; }
	// to be overridden by subclass
	// called before app quits
	// is called no matter if active or not
	virtual void die();
	// to be overridden by subclass
	virtual bool executeBusCommand(const QString &sCommand, const QJsonObject &ojoMessage);
	// to be overridden by subclass
	virtual QStringList exposedBusCommands() const;
	// to be overridden by subclass
	// prepare for possible GUI and other modules to then be able to connect
	// no matter if active or not, modules need to init
	virtual void init();
	inline bool isActive() const { return this->pMC->isOK() ? this->pMC->isActive() : false; }
	inline QString moduleUID() const { return this->pMC->moduleUID(); }
	// to be overridden by subclass
	// called after all modules and GUI have been initialized
	// is not called if not active
	virtual void start();
	// to be overridden by subclass
	// is called no matter if active or not
	virtual void stop();
	static QHostAddress stringToHostAddress(const QString &sHostOrIP);

signals:
	void busMessage(const QStringList &asUIDs, const QJsonObject &ojoMessage) const;
	void debugMessage(const QString &sMessage) const;

public slots:
	// to be overridden by subclass
	virtual void onBusMessage(const QJsonObject &ojoMessage);
	inline virtual void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("ModuleBase:" + sMessage); }

}; // ModuleBase



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_MODULEBASE_H
