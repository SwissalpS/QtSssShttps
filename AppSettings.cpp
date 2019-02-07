#include "AppSettings.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>
#include <QMutex>
#include <QStandardPaths>



namespace SwissalpS { namespace QtSssSapp {



const QString AppSettings::sSettingHTTPSpathCert = "sHTTPSpathCert";
const QString AppSettings::sSettingHTTPSpathKey = "sHTTPSpathKey";
const QString AppSettings::sSettingInteractive = "bInteractive";
const QString AppSettings::sSettingPathBase = "sPathBase"; // not in ini file
const QString AppSettings::sSettingPIDpathFile = "sPathFilePID";
const QString AppSettings::sSettingVerbose = "bVerbose";

#ifdef SssS_USE_GUI
const QString AppSettings::sSettingUseGUI = "bUseGUI";
#endif


const QString AppSettings::sSettingHTTPSpathCertDefault = "";
const QString AppSettings::sSettingHTTPSpathKeyDefault = "";
const bool AppSettings::bSettingInteractiveDefault = false;
const QString AppSettings::sSettingPIDpathFileDefault = "";
const bool AppSettings::bSettingVerboseDefault = false;

#ifdef SssS_USE_GUI
const bool AppSettings::sSettingUseGUIdefault = true;
#endif


AppSettings *AppSettings::pSingelton = nullptr;


AppSettings::AppSettings(QObject *parent) :
	QObject(parent),
	sPathDataBase(""),
	pSettings(nullptr),
	bInitDone(false) {

	this->apModuleConfs.clear();

	this->oJo = QJsonObject();

} // construct


AppSettings::~AppSettings() {

} // dealloc


AppSettings *AppSettings::pAppSettings() {

	///
	/// \brief oMutex
	///
	static QMutex oMutex;

	///////////////////////////////
	/* NOTE from: http://www.qtcentre.org/wiki/index.php?title=Singleton_pattern
	 * Be aware that you should check for a std::bad_alloc exception
	 * to unlock the mutex in case new fails. We are also using
	 * double-checked locking as locking and unlocking a mutex
	 * is expensive and would significantly slow down a call to
	 *  Singleton::instance().
	 */
	if (!AppSettings::pSingelton) {

		oMutex.lock();

		if (!pSingelton) {

			pSingelton = new AppSettings();

		} // if first call

		oMutex.unlock();

	} // if first call

	return pSingelton;

} // singelton access


void AppSettings::drop() {

	static QMutex oMutex;

	oMutex.lock();

	delete pSingelton;
	pSingelton = 0;

	oMutex.unlock();

} // drop singelton


void AppSettings::copyResources(QString sPathSource, QString sPathTarget, bool bExe) {
	//qDebug() << __PRETTY_FUNCTION__ << sPathSource << sPathTarget;

	QDir oPath = QDir("/");
	// first make sure target path exists
	if (!oPath.mkpath(sPathTarget)) {

		this->onDebugMessage(tr("Can not make path: ") + sPathTarget +
						"\n" + tr("Without it, I can not predict what will happen."));

		Q_EMIT this->dataDirError();

		return;

	} // if path does not exist and can not be made

	// ensure a trailing slash
	if (!sPathTarget.endsWith("/")) sPathTarget += "/";
	if (!sPathSource.endsWith("/")) sPathSource += "/";
	if (!sPathSource.startsWith(":/")) {

		if (sPathSource.startsWith("/")) {

			sPathSource = ":" + sPathSource;

		} else sPathSource = ":/" + sPathSource;

	} // if not a full source path

	QString sResource;
	QString sPathFileSource, sPathFileTarget;
	foreach (sResource, QDir(sPathSource).entryList()) {

		sPathFileSource = sPathSource + sResource;
		sPathFileTarget = sPathTarget + sResource;

		if (QFileInfo(sPathFileSource).isDir()) {

			this->copyResources(sPathFileSource, sPathFileTarget, bExe);

			continue;

		} // if dir encountered

		if (!QFileInfo(sPathFileTarget).exists()) {

			QFile::copy(sPathFileSource, sPathFileTarget);

			// set correct permissions
			if (bExe) {

				QFile(sPathFileTarget).setPermissions(
						QFile::ReadOther | QFile::ReadGroup
						| QFile::ReadOwner
						| QFile::WriteGroup | QFile::WriteOwner
						| QFile::ExeGroup | QFile::ExeOwner);

			} else {

				QFile(sPathFileTarget).setPermissions(
						QFile::ReadOther | QFile::ReadGroup
						| QFile::ReadOwner
						| QFile::WriteGroup | QFile::WriteOwner);

			} // if make executable or not

		} // if resource file does not exist in target directory

	} // loop all resources source directory

} // copyResources


QVariant AppSettings::get(const QString sKey) const {

	// if init has run
	if (this->bInitDone) {

		// return override value if exists
		if (this->oJo.contains(sKey)) return this->oJo.value(sKey).toVariant();

	} // if init done

	QString sPathFile;

	if (sSettingHTTPSpathCert == sKey) {

		sPathFile = pSettings->value(sKey,
									 sSettingHTTPSpathCertDefault).toString().trimmed();

		if (sPathFile.isEmpty()) {

			sPathFile = this->sPathDataBase + "sslCert.pem";

		} // if none defined

		return QVariant(sPathFile);

	} else if (sSettingHTTPSpathKey == sKey) {

		sPathFile = pSettings->value(sKey,
									 sSettingHTTPSpathKeyDefault).toString().trimmed();

		if (sPathFile.isEmpty()) {

			sPathFile = this->sPathDataBase + "sslKey";

		} // if none defined

		return QVariant(sPathFile);

	} else if (sSettingInteractive == sKey) {

		return this->pSettings->value(sKey, bSettingInteractiveDefault);

	} else if (sSettingPathBase == sKey) {

		return QVariant(this->sPathDataBase);

	} else if (sSettingPIDpathFile == sKey) {

		sPathFile = pSettings->value(
								sKey, sSettingPIDpathFileDefault).toString().trimmed();

		if (sPathFile.isEmpty()) {

			sPathFile = this->sPathDataBase + "QtSssSmultimedia.pid";

		} // if none defined

		return QVariant(sPathFile);

	}
#ifdef SssS_USE_GUI
	else if (sSettingUseGUI == sKey) {

		return pSettings->value(sKey, sSettingUseGUIdefault);

	}
#endif
	else if (sSettingVerbose == sKey) {

		return pSettings->value(sKey, bSettingVerboseDefault);

	} else {

		this->onDebugMessage(tr("OO:Unhandled key: ") + sKey);

		return this->pSettings->value(sKey);

	} // switch sKey

} // get


QString AppSettings::getPathBin() const {

	return this->sPathDataBase + "bin/";

} // getPathBin


QString AppSettings::getPathData() const {

	return this->sPathDataBase;

} // getPathData


QString AppSettings::getPathModules() const {

	return this->sPathDataBase + "modules.d/";

} // getPathModules


QString AppSettings::getPathModuleTemplates() const {

	return this->sPathDataBase + "moduleTemplates/";

} // getPathModuleTemplates


QSettings *AppSettings::getSettings() const {

	return this->pSettings;

} // getSettings


void AppSettings::init() {

	if (this->bInitDone) return;

	// parse command line arguments
	this->initOverrides();

	// init paths
	this->initPaths();

	// init directory structure
	this->setupDirs(this->sPathDataBase);

	// read all configurations
	this->readModules();

	// init settings

	this->pSettings = new QSettings(this->sPathDataBase + "Settings.ini",
							  QSettings::IniFormat, this);

	QSettings *pS = this->pSettings;

	pS->setValue(sSettingHTTPSpathCert, get(sSettingHTTPSpathCert));
	pS->setValue(sSettingHTTPSpathKey, get(sSettingHTTPSpathKey));
	pS->setValue(sSettingInteractive, this->get(sSettingInteractive));
	pS->setValue(sSettingPIDpathFile, get(sSettingPIDpathFile));
	pS->setValue(sSettingVerbose, this->get(sSettingVerbose));

#ifdef SssS_USE_GUI
	pS->setValue(sSettingUseGUI, get(sSettingUseGUI));
#endif

	this->pSettings->sync();

	this->bInitDone = true;

} // init


void AppSettings::initOverrides() {

	QCommandLineOption oCert("cert", "Local path to SSL-certificate", "path/to/cert.pem");
	QCommandLineOption oKey("key", "Local path to SSL-key", "path/to/rsa.key");
	//QCommandLineOption oIP("ip", "Local IP to use.", "IP");
	QCommandLineOption oInteractive(QStringList() << "i" << "interactive", "Run with command line interface.");
	QCommandLineOption oPath("base-path", "Local path to use for settings etc.", "path");
	//QCommandLineOption oPort("port", "Local port to use.", "port");
	QCommandLineOption oVerbose(QStringList() << "V" << "verbose", "Be verbose.");
#ifdef SssS_USE_GUI
	QCommandLineOption oGUI(QStringList() << "G" << "withGUI", "Use GUI.");
	QCommandLineOption oNoGUI(QStringList() << "g" << "withoutGUI", "No GUI. Over-rides -G");
//	QCommandLineOption (QStringList() << "g" << "gui", "Show GUI.");
//	QCommandLineOption ("no-gui", "Don't show GUI.");
#endif

	QCommandLineParser oCLP;
	oCLP.setApplicationDescription("Qt SwissalpS Application.");
	oCLP.addHelpOption();
	oCLP.addVersionOption();

	oCLP.addOption(oCert);
	oCLP.addOption(oKey);
//	oCLP.addOption(oIP);
	oCLP.addOption(oInteractive);
	oCLP.addOption(oPath);
//	oCLP.addOption(oPort);
	oCLP.addOption(oVerbose);
#ifdef SssS_USE_GUI
	oCLP.addOption(oGUI);
	oCLP.addOption(oNoGUI);
#endif

	// parse command line arguments
	oCLP.process(*QCoreApplication::instance());

	const QString sPath = oCLP.value(oPath).trimmed();
	const QString sPathCert = oCLP.value(oCert);
	const QString sPathKey = oCLP.value(oKey);
//	const QString sIP = oCLP.value(oIP);
	const bool bInteractive = oCLP.isSet(oInteractive);
//	const quint16 uiPort = oCLP.value(oPort).toUInt();
	const bool bVerbose = oCLP.isSet(oVerbose);
#ifdef SssS_USE_GUI
	bool bGUI = oCLP.isSet(oGUI);
	bool bNoGUI = oCLP.isSet(oNoGUI);
#endif

	// TODO: check validity!

	if (!sPath.isEmpty()) {
		this->oJo.insert(sSettingPathBase, sPath);
	} // if path base

	if (!sPathCert.isEmpty()) {
		this->oJo.insert(sSettingHTTPSpathCert, sPathCert);
	} // if path cert

	if (!sPathKey.isEmpty()) {
		this->oJo.insert(sSettingHTTPSpathKey, sPathKey);
	} // if path key

//	if (!sIP.isEmpty()) {
//		this->oJo.insert(sSettingHTTPSip, sIP);
//	} // if IP

//	if (uiPort) {
//		this->oJo.insert(sSettingHTTPSport, (double)uiPort);
//	} // if port

	if (bVerbose) {
		this->oJo.insert(sSettingVerbose, true);
	} // if verbose

#ifdef SssS_USE_GUI
	if (bNoGUI) {
		this->oJo.insert(sSettingUseGUI, false);
	} else if (bGUI) {
		this->oJo.insert(sSettingUseGUI, true);
	} // if GUI or not
#endif

	// needs to be after verbose and gui switches
	if (bInteractive) {
		this->oJo.insert(sSettingInteractive, true);
		this->oJo.insert(sSettingVerbose, false);
#ifdef SssS_USE_GUI
		this->oJo.insert(sSettingUseGUI, false);
#endif
	} // if interactive cli

} // initOverrides


void AppSettings::initPaths() {

	if (this->isOverridden(sSettingPathBase)) {

		// init overrides has stored the value
		const QString sPath = this->oJo.value(sSettingPathBase).toString();
		this->sPathDataBase = sPath + (sPath.endsWith("/") ? "" : "/")
							  + QCoreApplication::applicationName() + "/";

	} else {

		// data ie playlists, settings etc
		const QStringList aPaths = QStandardPaths::standardLocations(
								 QStandardPaths::DocumentsLocation);

		this->sPathDataBase = aPaths.first() + "/"
							  + QCoreApplication::applicationName() + "/";

	} // if given path or not

	this->onDebugMessage(tr("OK:Using for data: ") + this->sPathDataBase);

} // initPaths


// modules are read in order that the system sorts by name
// on some systems 11.json will come before 2.json
// so to be safe, name the configs 01_second.json, 02_third.json, 11_twelveth.json
void AppSettings::readModules() {

	this->onDebugMessage("readModules");

	this->apModuleConfs.clear();

	QString sBase = this->getPathModules();
	QStringList aFiles = QDir(sBase).entryList(
							 QStringList() << "*.json",
							 QDir::Files | QDir::Readable | QDir::Writable
							 | QDir::NoDotAndDotDot,
							 QDir::LocaleAware | QDir::Name);

	QString sFile;
	ModuleConf *pMC;
	for (int i = 0; i < aFiles.length(); ++i) {

		sFile = aFiles.at(i);

		this->onDebugMessage(tr("OK:reading module config: ") + sFile);

		pMC = new ModuleConf(sBase + sFile, this);

		connect(pMC, SIGNAL(debugMessage(QString)),
				this, SLOT(onDebugMessage(QString)));

		this->apModuleConfs.append(pMC);

	} // loop

} // readModules


void AppSettings::setSettings(QSettings *pNewSettings) {

	this->pSettings = pNewSettings;

} // setSettings


void AppSettings::setupDirs(QString sPathBase) {

	QDir oPath = QDir(sPathBase);

	if (!oPath.mkpath(sPathBase)) {

		this->onDebugMessage(tr("KO:Can not make path: ") + sPathBase +
						"\n" + tr("Without it, I can not predict what will happen."));

		Q_EMIT this->dataDirError();

		return;

	} // if path does not exist and can not be made

	// make sure path exists
	oPath.mkpath(this->getPathModules());

	// make sure templates path exists
	oPath.mkpath(this->getPathModuleTemplates());
	this->copyResources(":/moduleTemplates/", this->getPathModuleTemplates());

	// also make a folder for executables and copy missing resources
	QString sPathBin = sPathBase + "bin/";
	this->copyResources(":/bin/", sPathBin, true);

} // setupDirs



}	} // namespace SwissalpS::QtSssSapp
