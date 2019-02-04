#ifndef SwissalpS_QtSssSapp_MODULECONF_H
#define SwissalpS_QtSssSapp_MODULECONF_H

#include <QObject>
#include <QJsonObject>

#include "PersistantObject.h"


namespace SwissalpS { namespace QtSssSapp {



class ModuleConf : public PersistantObject {

	Q_OBJECT
	Q_DISABLE_COPY(ModuleConf)

private:

protected:
	const QString sTagActive = QStringLiteral("bActive");
	const QString sTagClass = QStringLiteral("sClass");
	const QString sTagListenIP = QStringLiteral("sListenIPorHostname");
	const QString sTagListenPort = QStringLiteral("uiListenPort");
	const QString sTagListenPort1 = QStringLiteral("uiListenPort1");
	const QString sTagListenPort2 = QStringLiteral("uiListenPort2");
	const QString sTagListIP = QStringLiteral("aListIP");
	const QString sTagListIPisBlackNotWhite = QStringLiteral("bListIPisBlackNotWhite");
	const QString sTagMaxConnections = QStringLiteral("uiMaxConnections");
	const QString sTagPassword = QStringLiteral("sPassword");
	const QString sTagPathCert = QStringLiteral("sPathCert");
	const QString sTagPathKey = QStringLiteral("sPathKey");
	const QString sTagTargetIP = QStringLiteral("sTargetIPorHostname");
	const QString sTagTargetPort = QStringLiteral("uiTargetPort");
	const QString sTagTargetPort1 = QStringLiteral("uiTargetPort1");
	const QString sTagTargetPort2 = QStringLiteral("uiTargetPort2");
	const QString sTagUID = QStringLiteral("sUID");
	const QString sTagUsername = QStringLiteral("sUser");

public:
	static const QString sModuleAppControl;
	static const QString sModuleZeroConfig;

	explicit ModuleConf(const QString sPath, QObject *pParent = nullptr);
	virtual ~ModuleConf();

	virtual bool isActive() const;
	virtual bool isListBlack() const;
	virtual QStringList listBW() const;
	virtual QString localIP() const;
	virtual quint16 localPort() const;
	virtual quint16 localPort1() const;
	virtual quint16 localPort2() const;
	virtual quint16 maxConnections() const;
	virtual QString moduleClass() const;
	virtual QString moduleUID() const;
	virtual QString password() const;
	virtual QString pathCert() const;
	virtual QString pathKey() const;

	virtual void setActive(const bool bActive);
	virtual void setListBlackOrWhite(const bool bBlackNotWhite);
	virtual void setListBW(const QStringList &asList);
	virtual void setLocalIP(const QString &sHost);
	virtual void setLocalPort(const quint16 uiPort);
	virtual void setLocalPort1(const quint16 uiPort);
	virtual void setLocalPort2(const quint16 uiPort);
	virtual void setMaxConnections(const quint16 uiMax);
	virtual void setModuleClass(const QString &sClass);
	virtual void setModuleUID(const QString &sUID);
	virtual void setPassword(const QString &sPassword);
	virtual void setPathCert(const QString &sPathCert);
	virtual void setPathKey(const QString &sPathKey);
	virtual void setTargetIP(const QString &sHost);
	virtual void setTargetPort(const quint16 uiPort);
	virtual void setTargetPort1(const quint16 uiPort);
	virtual void setTargetPort2(const quint16 uiPort) ;
	virtual void setUser(const QString &sUser);

	virtual QString targetIP() const;
	virtual quint16 targetPort() const;
	virtual quint16 targetPort1() const;
	virtual quint16 targetPort2() const;
	virtual QString user() const;
signals:

public slots:

}; // ModuleConf



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_MODULECONF_H
