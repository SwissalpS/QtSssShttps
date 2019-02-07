#ifndef SwissalpS_QtSssSapp_APPSETTINGS_H
#define SwissalpS_QtSssSapp_APPSETTINGS_H

#include <QObject>
#include <QSettings>
#include <QJsonObject>
#include <QVector>

#include "ModuleConf.h"



namespace SwissalpS { namespace QtSssSapp {



class AppSettings : public QObject {

	Q_OBJECT
	Q_DISABLE_COPY(AppSettings)

private:
	QString sPathDataBase;
	QSettings *pSettings;

	static AppSettings *pSingelton;

	// keep this private as we want only one instance
	// read; http://www.qtcentre.org/wiki/index.php?title=Singleton_pattern
	explicit AppSettings(QObject *pParent = 0);

	void copyResources(QString sPathSource, QString sPathTarget, bool bExe = false);
	void setupDirs(QString sPathBase);

protected:
	QVector<ModuleConf *> apModuleConfs;
	QJsonObject oJo; // contains overrides from CLI arguments
	bool bInitDone;
	void initOverrides();
	void initPaths();
	void readModules();

public:
	static const QString sSettingHTTPSpathCert;
	static const QString sSettingHTTPSpathKey;
	static const QString sSettingHTTPSip;
	static const QString sSettingHTTPSport;
	static const QString sSettingInteractive;
	static const QString sSettingPathBase; // this one is not in settings.ini
	static const QString sSettingPIDpathFile;
	static const QString sSettingVerbose;

#ifdef SssS_USE_GUI
	static const QString sSettingUseGUI;
#endif

	static const QString sSettingHTTPSpathCertDefault;
	static const QString sSettingHTTPSpathKeyDefault;
	static const QString sSettingHTTPSipDefault;
	static const quint16 uSettingHTTPSportDefault;
	static const bool bSettingInteractiveDefault;
	static const QString sSettingPIDpathFileDefault;
	static const bool bSettingVerboseDefault;

#ifdef SssS_USE_GUI
	static const bool sSettingUseGUIdefault;
#endif

	// destroy singelton
	static void drop();
	// public access to singelton instance
	static AppSettings *pAppSettings();
	virtual ~AppSettings();

	QVariant get(const QString sKey) const;

	QString getPathBin() const;
	QString getPathData() const;
	QString getPathModules() const;
	QString getPathModuleTemplates() const;

	QSettings *getSettings() const;
	void setSettings(QSettings *pNewSettings);

	void init();
	inline bool isOverridden(const QString sKey) { return this->oJo.contains(sKey); }
	inline bool isReady() const { return this->bInitDone; }
	inline QVector<ModuleConf *> moduleConfs() const { return this->apModuleConfs; }

signals:
	void debugMessage(const QString &sMessage) const;
	void dataDirError() const;

public slots:
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("AS:" + sMessage); }

	inline void setValue(const QString &sKey, const QVariant &mValue) {
		if (this->isOverridden(sKey)) this->oJo.remove(sKey);
		this->pSettings->setValue(sKey, mValue); }

	inline void sync() { this->pSettings->sync(); }

}; // AppSettings



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_APPSETTINGS_H
