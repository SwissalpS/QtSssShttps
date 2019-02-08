#include "AppController.h"

#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QMutex>
#include <QTimer>
#include <iostream>

#include "ModuleAppControl.h"
#include "ModuleHTTPSserver.h"
#include "ModuleZeroConfig.h"

#ifdef SssS_USE_GUI
//#include "GUImainFrame.h"
#endif



namespace SwissalpS { namespace QtSssSapp {



AppController *AppController::pSingelton = nullptr;

AppController::AppController(QObject *pParent) :
	QObject(pParent),
	pAS(nullptr),
	pCLI(nullptr),
	pTimerShutdown(nullptr) {

	this->asModules.clear();
	this->hpModules.clear();

} // construct


AppController::~AppController() {

} // dealloc


void AppController::drop() {

	static QMutex oMutex;

	oMutex.lock();

	delete pSingelton;
	pSingelton = 0;

	oMutex.unlock();

} // drop


AppController *AppController::pAppController() {

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
	if (!AppController::pSingelton) {

		oMutex.lock();

		if (!pSingelton) {

			pSingelton = new AppController();

		} // if first call

		oMutex.unlock();

	} // if first call

	return pSingelton;

} // singelton access


void AppController::checkDependencies() {
} // checkDependencies


#ifdef SssS_USE_GUI
void AppController::initGUI() {
} // initGUI
#endif


void AppController::initModules() {

	QString sUID;
	ModuleBase *pModule;
	for (int i = 0; i < this->asModules.length(); ++i) {

		sUID = this->asModules.at(i);
		pModule = this->hpModules.value(sUID);

		this->onDebugMessage(tr("OK:Initializing module: ") + sUID);
		pModule->init();
		this->onDebugMessage(tr("OK:Initialized module: ") + sUID);

	} // loop

	Q_EMIT this->modulesInitialized();

} // initModules


void AppController::initRegistration() {

	QString sPathFile = this->pAS->getPathData() + "Registration.data";

	bool bHasValidRegistration = false;

	int iDelay = 3 * 60000;

	if (QFile::exists(sPathFile)) {

		iDelay = 5 * 60000;

		QFile oFile(sPathFile);
		if (oFile.open(QIODevice::ReadOnly)) {

			QByteArray oData = oFile.readAll();
			oFile.close();

			QByteArray oDataDecoded = QByteArray::fromBase64(oData);

			QString sReg = "Unlocked";

			// to make a new code:
			//QByteArray oReg(sReg.toUtf8()); qDebug() << oReg.toBase64();

			if (0 == QString::fromUtf8(oDataDecoded).compare(sReg)) {

				bHasValidRegistration = true;

			} // if valid hash found

		} // if could open file

	} // if file exists

	if (bHasValidRegistration) return;

	startTimer(iDelay);

	std::cout << tr("You are running an unregistered beta-version.\n"
					"It will quit after a couple minutes.").toStdString();
	std::cout << std::endl;

} // initRegistration


void AppController::initSettings() {

	this->pAS = AppSettings::pAppSettings();

	connect(this->pAS, SIGNAL(debugMessage(QString)),
			this, SLOT(onDebugMessage(QString)));

	this->pAS->init();

} // initSettings


// kill in reverse order
void AppController::killModules() {

	QString sUID;
	ModuleBase *pModule;
	for (int i = this->asModules.length() -1; 0 <= i; i--) {

		sUID = this->asModules.at(i);
		pModule = this->hpModules.value(sUID);

		pModule->die();

		this->hpModules.remove(sUID);
		pModule->deleteLater();

		this->onDebugMessage(tr("OK:Killed module: ") + sUID);

	} // loop

	this->asModules.clear();
	// this is just for good practice
	this->hpModules.clear();

} // killModules


// instantiate the actual module-objects
void AppController::loadModules() {

	if (this->asModules.length()) {

		// oops already loaded, for now skip
		this->onDebugMessage(tr("KO:AppController::loadModules called after modules already loaded."));
		return;

	} // if already loaded modules

	this->asModuleLoadErrorMessages.clear();
	this->asModules.clear();
	this->hpModules.clear();

	QVector<ModuleConf *> apMCs = this->pAS->moduleConfs();
	// place-holder for iteration
	ModuleConf *pMC;

	// place-holders for all valid module-classes
	ModuleAppControl *pMAC;
	ModuleBase *pMB;
	ModuleHTTPSserver *pMHTTPSS;
	ModuleZeroConfig *pMZC;

	QString sUID;
	QString sUIDorig;
	QString sClass;
	QString sMessage;
	for (int i = 0; i < apMCs.length(); ++i) {

		pMC = apMCs.at(i);

		if (!pMC->isOK()) {

			sMessage = tr("KO:could not load module at: ") + pMC->path();
			this->asModuleLoadErrorMessages.append(sMessage);
			this->onDebugMessage(sMessage);
			continue;

		} // if did not load properly

		sClass = pMC->moduleClass();

		// make sure UID is unique
		sUID = sUIDorig = pMC->moduleUID();
		while (this->hpModules.contains(sUID)) sUID += ".";
		if (0 != sUID.compare(sUIDorig)) {

			sMessage = tr("KO:duplicate UID changed (old:new): ")
					   + sUIDorig + ":" + sUID + " " + tr("in file: ")
					   + pMC->path();

			this->asModuleLoadErrorMessages.append(sMessage);
			this->onDebugMessage(sMessage);

			pMC->setModuleUID(sUID);

		} // if needed to adapt UID

		this->onDebugMessage(tr("OK:Loading module: ") + sUID + ":" + sClass);

		if (0 == sClass.compare(ModuleConf::sModuleAppControl)) {

			pMAC = new ModuleAppControl(pMC, this);
			this->hpModules.insert(sUID, pMAC);
			this->asModules.append(sUID);

			pMB = pMAC;

//			connect(pMAC, SIGNAL(busMessage(QStringList,QJsonObject)),
//					this, SLOT(onBusMessage(QStringList,QJsonObject)));

//			connect(pMAC, SIGNAL(debugMessage(QString)),
//					this, SLOT(onDebugMessage(QString)));

//			connect(pMAC, SIGNAL(registerZeroConfServiceDescriptor(ZeroConfServiceDescriptor*)),
//					this, SLOT(onRegisterZeroConfServiceDescriptor(ZeroConfServiceDescriptor*)));

//			connect(pMAC, SIGNAL(startZeroConfServices(QString)),
//					this, SLOT(onStartZeroConfServices(QString)));

//			connect(pMAC, SIGNAL(stopZeroConfServices(QString)),
//					this, SLOT(onStopZeroConfServices(QString)));

//			connect(pMAC, SIGNAL(unregisterZeroConfServiceDescriptors(QString)),
//					this, SLOT(onUnregisterZeroConfServiceDescriptors(QString)));

		} else if (0 == sClass.compare(ModuleConf::sModuleBase)) {

			// can be used to only use for ZeroConf publishing of
			// services provided by other processes
			pMB = new ModuleBase(pMC, this);
			this->hpModules.insert(sUID, pMB);
			this->asModules.append(sUID);

		} else if (0 == sClass.compare(ModuleConf::sModuleHTTPSserver)) {

			pMHTTPSS = new ModuleHTTPSserver(pMC, this);
			this->hpModules.insert(sUID, pMHTTPSS);
			this->asModules.append(sUID);
			pMB = pMHTTPSS;

//			connect(pMHTTPSS, SIGNAL(busMessage(QStringList,QJsonObject)),
//					this, SLOT(onBusMessage(QStringList,QJsonObject)));

//			connect(pMHTTPSS, SIGNAL(debugMessage(QString)),
//					this, SLOT(onDebugMessage(QString)));

//			connect(pMHTTPSS, SIGNAL(registerZeroConfServiceDescriptor(ZeroConfServiceDescriptor*)),
//					this, SLOT(onRegisterZeroConfServiceDescriptor(ZeroConfServiceDescriptor*)));

//			connect(pMHTTPSS, SIGNAL(startZeroConfServices(QString)),
//					this, SLOT(onStartZeroConfServices(QString)));

//			connect(pMHTTPSS, SIGNAL(stopZeroConfServices(QString)),
//					this, SLOT(onStopZeroConfServices(QString)));

//			connect(pMHTTPSS, SIGNAL(unregisterZeroConfServiceDescriptors(QString)),
//					this, SLOT(onUnregisterZeroConfServiceDescriptors(QString)));

		} else if (0 == sClass.compare(ModuleConf::sModuleZeroConfig)) {

			pMZC = new ModuleZeroConfig(pMC, this);

			connect(pMZC, SIGNAL(busMessage(QStringList,QJsonObject)),
					this, SLOT(onBusMessage(QStringList,QJsonObject)));

			connect(pMZC, SIGNAL(debugMessage(QString)),
					this, SLOT(onDebugMessage(QString)));

			connect(pMZC, SIGNAL(registerZeroConfServiceDescriptor(ZeroConfServiceDescriptor*)),
					pMZC, SLOT(onAddServiceDescriptor(ZeroConfServiceDescriptor*)));

			connect(pMZC, SIGNAL(serviceError()),
					this, SLOT(onZeroConfServiceError()));

			connect(pMZC, SIGNAL(serviceErrorNameCollision()),
					this, SLOT(onServiceErrorNameCollision()));

			connect(pMZC, SIGNAL(serviceErrorRegistration()),
					this, SLOT(onServiceErrorRegistration()));

			connect(pMZC, SIGNAL(startZeroConfServices(QString)),
					pMZC, SLOT(onStartZeroConfServices(QString)));

			connect(pMZC, SIGNAL(stopZeroConfServices(QString)),
					pMZC, SLOT(onStopZeroConfServices(QString)));

			connect(pMZC, SIGNAL(unregisterZeroConfServiceDescriptors(QString)),
					pMZC, SLOT(onRemoveServiceDescriptors(QString)));

			connect(this, SIGNAL(registerZeroConfServiceDescriptor(ZeroConfServiceDescriptor*)),
					pMZC, SLOT(onAddServiceDescriptor(ZeroConfServiceDescriptor*)));

			connect(this, SIGNAL(startZeroConfServices(QString)),
					pMZC, SLOT(onStartZeroConfServices(QString)));

			connect(this, SIGNAL(stopZeroConfServices(QString)),
					pMZC, SLOT(onStopZeroConfServices(QString)));

			connect(this, SIGNAL(unregisterZeroConfServiceDescriptors(QString)),
					pMZC, SLOT(onRemoveServiceDescriptors(QString)));

			this->hpModules.insert(sUID, pMZC);
			this->asModules.append(sUID);

			this->onDebugMessage(tr("OK:Loaded module: ") + sUID + ":" + sClass);

			continue;

		} else {

			sMessage = tr("KO:Unknown module class: ") + sClass
					   + " " + tr("in: ") + pMC->path();

			this->asModuleLoadErrorMessages.append(sMessage);
			this->onDebugMessage(sMessage);

			continue;

		} // switch class

		connect(pMB, SIGNAL(busMessage(QStringList,QJsonObject)),
				this, SLOT(onBusMessage(QStringList,QJsonObject)));

		connect(pMB, SIGNAL(debugMessage(QString)),
				this, SLOT(onDebugMessage(QString)));

		connect(pMB, SIGNAL(registerZeroConfServiceDescriptor(ZeroConfServiceDescriptor*)),
				this, SLOT(onRegisterZeroConfServiceDescriptor(ZeroConfServiceDescriptor*)));

		connect(pMB, SIGNAL(startZeroConfServices(QString)),
				this, SLOT(onStartZeroConfServices(QString)));

		connect(pMB, SIGNAL(stopZeroConfServices(QString)),
				this, SLOT(onStopZeroConfServices(QString)));

		connect(pMB, SIGNAL(unregisterZeroConfServiceDescriptors(QString)),
				this, SLOT(onUnregisterZeroConfServiceDescriptors(QString)));

		this->onDebugMessage(tr("OK:Loaded module: ") + sUID + ":" + sClass);

	} // loop

} // loadModules


void AppController::onBusMessage(const QStringList &asUIDs,
								 const QJsonObject &ojoMessage) {

	// no targets => all targets
	if (asUIDs.isEmpty()) {

		for (int i = 0; i < this->asModules.length(); ++i) {

			this->hpModules.value(this->asModules.at(i))->onBusMessage(ojoMessage);

		} // loop all modules

		return;

	} // if no targets given

	QString sUID;
	for (int i = 0; i < asUIDs.length(); ++i) {

		sUID = asUIDs.at(i);

		if (this->hpModules.contains(sUID)) {

			this->hpModules.value(sUID)->onBusMessage(ojoMessage);

		} else this->onDebugMessage("KO:Got bus message for unknown UID: " + sUID);

	} // loop given UIDs

} // onBusMessage


void AppController::onDebugMessage(const QString &sMessage) {

	// for now we default to verbose while pAS is getting ready
	//if ((!this->pAS) || (!this->pAS->isReady()) || this->pAS->get(AppSettings::sSettingVerbose).toBool()) {

	// only output to std out if ready and is requested that way
	if ((this->pAS) && (this->pAS->isReady())
			&& this->pAS->get(AppSettings::sSettingVerbose).toBool()) {

		std::cout << ":" << sMessage.toStdString();
		std::cout << std::endl;

	} // if verbose at all

	// allow others to log or display messages
	Q_EMIT debugMessageReceived(sMessage);

} // onDebugMessage


void AppController::quit() {

	if (this->pTimerShutdown) {
		this->pTimerShutdown->stop();
		this->pTimerShutdown->deleteLater();
		this->pTimerShutdown = nullptr;
	}

	this->pAS->sync();

	this->stopModules();

	this->killModules();

	// TODO: check if we can skip one or the other
	Q_EMIT this->done();

	//qApp->quit();

} // quit


void AppController::run() {

	// protect against multiple calls
	static bool bFirstRun = true;
	if (!bFirstRun) return;
	bFirstRun = false;

	// init settings
	this->initSettings();

	// TODO: check for already running instance
	this->writePID();

	// check dependencies
	this->checkDependencies();

	this->loadModules();
	// TODO: check that ports are free
	this->initModules();

	bool bInteractive = this->pAS->get(AppSettings::sSettingInteractive).toBool();

	// give gui a chance to init and connect
#ifdef SssS_USE_GUI
	if (!bInteractive) {
		// init GUI
		this->initGUI();
	} // if not interactive CLI-mode
#endif

	this->startModules();

	if (bInteractive) this->startCLI();

} // run


void AppController::startCLI() {

	this->pCLI = new CLIresponder(this->pAS, this);

	connect(this->pCLI, SIGNAL(debugMessage(QString)),
			this, SLOT(onDebugMessage(QString)));

	this->pCLI->start();

} // startCLI


void AppController::startModules() {

	QString sUID;
	ModuleBase *pModule;
	for (int i = 0; i < this->asModules.length(); ++i) {

		sUID = this->asModules.at(i);
		pModule = this->hpModules.value(sUID);

		if (!pModule->isActive()) continue;

		this->onDebugMessage(tr("OK:Starting module: ") + sUID);
		pModule->start();
		this->onDebugMessage(tr("OK:Started module: ") + sUID);

	} // loop

	Q_EMIT this->modulesStarted();

} // startModules


// stop in reverse order
void AppController::stopModules() {

	QString sUID;
	ModuleBase *pModule;
	for (int i = this->asModules.length() -1; 0 <= i; i--) {

		sUID = this->asModules.at(i);
		pModule = this->hpModules.value(sUID);

		this->onDebugMessage(tr("OK:Stopping module: ") + sUID);
		pModule->stop();
		this->onDebugMessage(tr("OK:Stopped module: ") + sUID);

	} // loop

	Q_EMIT this->modulesStopped();

} // stopModules


void AppController::timerEvent(QTimerEvent *pEvent) {

	std::cout << tr("Please register, a lot of hard work went into the making.").toStdString();
	std::cout << std::endl;

	pEvent->accept();

	this->quit();

} // timerEvent


void AppController::writePID() {

	QString sPathPID = this->pAS->get(AppSettings::sSettingPIDpathFile).toString();
	QString sPID = QString::number(QCoreApplication::applicationPid());

	QFile oPID(sPathPID);
	if (!oPID.open(QIODevice::WriteOnly)) {

		this->onDebugMessage(tr("KO:Cannot write to pid-file: ")
							 + sPathPID);

		return;

	} // if failed to open pid-file

	oPID.write(sPID.toUtf8());
	oPID.close();

} // writePID



}	} // namespace SwissalpS::QtSssSapp

