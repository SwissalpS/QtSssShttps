#ifndef SwissalpS_QtSssSapp_MODULEZEROCONFIG_H
#define SwissalpS_QtSssSapp_MODULEZEROCONFIG_H

#include <QAbstractSocket>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QVector>

#include "ModuleBase.h"



namespace SwissalpS { namespace QtSssSapp {



class ModuleZeroConfig : public ModuleBase {

	Q_OBJECT
	Q_DISABLE_COPY(ModuleZeroConfig)

private:
	bool bRunning;

protected:
	QVector<QZeroConf *> apZCservices;
	QVector<ZeroConfServiceDescriptor *> apZCdescriptors;

	void publish(ZeroConfServiceDescriptor *pZCSD);

public:
	ModuleZeroConfig(ModuleConf *pConf, QObject *pParent = nullptr);
	virtual ~ModuleZeroConfig();

	void die() override;
	bool executeBusCommand(const QString &sCommand, const QJsonObject &ojoMessage) override;
	QStringList exposedBusCommands() const override;
	void init() override;
	void start() override;
	void stop() override;

signals:
	// is emited in both cases in addition to another (serviceErrorNameCollision or serviceErrorRegistration)
	void serviceError() const;
	void serviceErrorNameCollision() const;
	void serviceErrorRegistration() const;

public slots:
	void onAddServiceDescriptor(ZeroConfServiceDescriptor *pZCSD);
	inline void onDebugMessage(const QString &sMessage) const override {
		Q_EMIT this->debugMessage("ModZC:" + sMessage); }

	void onRemoveServiceDescriptors(const QString &sUID);
	void onServiceError(QZeroConf::error_t eError);
	void onStartZeroConfServices(const QString &sUID);
	void onStopZeroConfServices(const QString &sUID);

}; // ModuleZeroConfig



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_MODULEZEROCONFIG_H
