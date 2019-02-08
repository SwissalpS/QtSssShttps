#ifndef SwissalpS_QtSssSapp_MODULECONF_H
#define SwissalpS_QtSssSapp_MODULECONF_H

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>

#include "PersistantObject.h"


namespace SwissalpS { namespace QtSssSapp {



class ModuleConf : public PersistantObject {

	Q_OBJECT
	Q_DISABLE_COPY(ModuleConf)

private:

protected:

public:
	static const QString sModuleAppControl;
	static const QString sModuleBase;
	static const QString sModuleHTTPSserver;
	static const QString sModuleZeroConfig;

	static const QString sTagActive;
	static const QString sTagClass;
	static const QString sTagListenIP;
	static const QString sTagListenPort;
	static const QString sTagListenPort1;
	static const QString sTagListenPort2;
	static const QString sTagListIP;
	static const QString sTagListIPisBlackNotWhite;
	static const QString sTagMaxConnections;
	static const QString sTagPassword;
	static const QString sTagPathCert;
	static const QString sTagPathKey;
	static const QString sTagTargetIP;
	static const QString sTagTargetPort;
	static const QString sTagTargetPort1;
	static const QString sTagTargetPort2;
	static const QString sTagUID;
	static const QString sTagUsername;
	static const QString sTagZeroConf;
	static const QString sTagZeroConfServiceActive;
	static const QString sTagZeroConfServiceDomain;
	static const QString sTagZeroConfServiceName;
	static const QString sTagZeroConfServicePort;
	static const QString sTagZeroConfServiceType;
	static const QString sTagZeroConfTXTrecord;
	static const QString sTagZeroConfTXTrecordLabel;
	static const QString sTagZeroConfTXTrecordValue;

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
	virtual void setZeroConf(const QJsonArray &ojaServiceDescriptors);

	virtual QString targetIP() const;
	virtual quint16 targetPort() const;
	virtual quint16 targetPort1() const;
	virtual quint16 targetPort2() const;
	virtual QString user() const;
	virtual QJsonArray zeroConf() const;

signals:

public slots:

}; // ModuleConf



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_MODULECONF_H
