#include "ModuleZeroConfig.h"



namespace SwissalpS { namespace QtSssSapp {



ModuleZeroConfig::ModuleZeroConfig(ModuleConf *pConf, QObject *pParent) :
	ModuleBase(pConf, pParent),
	bRunning(false) {

} // construct


ModuleZeroConfig::~ModuleZeroConfig() {

	this->die();

} // dealloc


void ModuleZeroConfig::onAddServiceDescriptor(ZeroConfServiceDescriptor *pZCSD) {

	if ((nullptr == pZCSD) || (pZCSD->isNull())) return;

	this->apZCdescriptors.append(pZCSD);

	// take ownership
	pZCSD->setParent(this);

	if (this->bRunning) this->publish(pZCSD);

} // addService


bool ModuleZeroConfig::executeBusCommand(const QString &sCommand,
								 const QJsonObject &ojoMessage) {

	return ModuleBase::executeBusCommand(sCommand, ojoMessage);


	// an empty sender uid is valid -> respond to all modules
	QString sSender = ojoMessage.value("sender").toString("");

	// missing respond command means we can't respond, which is not required at times
	QString sRespondCommand = ojoMessage.value("respondCommand").toString("NULL");
	bool bRespond = (0 != sRespondCommand.compare("NULL"));

	// prepare response object
	QJsonObject ojoResponse;

	if (0 == sCommand.compare("quit")) {

		this->pAC->quit();

		return true;

	} else {

		//this->onDebugMessage(tr("KO:Command not coded: " + sCommand));

		// give base a chance to execute
		return ModuleBase::executeBusCommand(sCommand, ojoMessage);

	} // switch command

	if (!bRespond) return true;

	ojoResponse.insert("sender", this->pMC->moduleUID());
	ojoResponse.insert("command", sRespondCommand);
	ojoResponse.insert("context", ojoMessage.value("context").toString(""));

	Q_EMIT this->busMessage(QStringList() << sSender, ojoResponse);

	return true;

} // executeCommand


QStringList ModuleZeroConfig::exposedBusCommands() const {

	static QStringList asList = ModuleBase::exposedBusCommands();

	//asList.append("");

	return asList;

} // exposedCommands


void ModuleZeroConfig::die() {

	ModuleBase::die();

	//this->stop();
	this->apZCservices.clear();

	qDeleteAll(this->apZCdescriptors);
	this->apZCdescriptors.clear();

} // die


void ModuleZeroConfig::init() {

	this->apZCdescriptors.clear();
	this->apZCservices.clear();

	ModuleBase::init();

} // init


void ModuleZeroConfig::publish(ZeroConfServiceDescriptor *pZCSD) {

	if ((nullptr == pZCSD) || pZCSD->isNull() || (!pZCSD->isActive())) return;

	QZeroConf *pService = new QZeroConf(this);
	this->apZCservices.append(pService);
	pZCSD->setServicePointer(pService);

	connect(pService, SIGNAL(error(QZeroConf::error_t)),
			this, SLOT(onServiceError(QZeroConf::error_t)));

	QJsonArray ojaTextRecords = pZCSD->textRecords();
	QJsonObject ojoTextRecord;
	QString sLabel;
	QString sValue;
	for (int i = 0; i < ojaTextRecords.count(); ++i) {

		ojoTextRecord = ojaTextRecords.at(i);

		sLabel = ojoTextRecord.value(ModuleConf::sTagZeroConfTXTrecordLabel).toString();
		sValue = ojoTextRecord.value(ModuleConf::sTagZeroConfTXTrecordValue).toString();
		if (sValue.isEmpty())
			pService->addServiceTxtRecord(sLabel);
		else
			pService->addServiceTxtRecord(sLabel, sValue);

	} // loop

	pService->startServicePublish(pZCSD->serviceName(), pZCSD->type(),
								  pZCSD->domain(), pZCSD->port());

} // publish


void ModuleZeroConfig::start() {

	// ignore if not active
	if (!this->isActive()) return;

	if (this->bRunning) this->stop();

	ModuleBase::start();

	this->apZCservices.clear();

	for (int i = 0; i < this->apZCdescriptors.length(); ++i) {

		this->publish(this->apZCdescriptors.at(i));

	} // loop

	this->bRunning = true;

} // start


void ModuleZeroConfig::stop() {

	ModuleBase::stop();

	for (int i = 0; i < this->apZCdescriptors.length(); ++i)
		this->apZCdescriptors.at(i)->setServicePointer();

	for (int i = 0; i < this->apZCservices.length(); ++i)
		this->apZCservices.at(i)->stopServicePublish();

	qDeleteAll(this->apZCservices);

	this->bRunning = false;

} // stop


void ModuleZeroConfig::onServiceError(QZeroConf::error_t eError) {

	if (QZeroConf::serviceNameCollision == eError)
		Q_EMIT this->serviceErrorNameCollision();

	else if (QZeroConf::serviceRegistrationFailed == eError)
		Q_EMIT this->serviceErrorRegistration();

	else return;

	Q_EMIT this->serviceError();

} // onServiceError


void ModuleZeroConfig::onStartZeroConfServices(const QString &sUID) {

	// make sure there are none running anymore
	this->onStopZeroConfServices(sUID);

	ZeroConfServiceDescriptor *pZCSD;
	for (int i = 0; i < this->apZCdescriptors.length(); ++i) {

		pZCSD = this->apZCdescriptors.at(i);
		if (0 == sUID.compare(pZCSD->moduleUID())) {

			this->publish(pZCSD);

		} // if found a match

	} // loop detect and start

} // onStartZeroConfServices


void ModuleZeroConfig::onStopZeroConfServices(const QString &sUID) {

	ZeroConfServiceDescriptor *pZCSD;
	QZeroConf *pService;
	for (int i = 0; i < this->apZCdescriptors.length(); ++i) {

		pZCSD = this->apZCdescriptors.at(i);
		if (0 == sUID.compare(pZCSD->moduleUID())) {

			pService = pZCSD->servicePointer();
			if (nullptr == pService) continue;
			pZCSD->setServicePointer();

			this->apZCservices.removeOne(pService);

			pService->stopServicePublish();
			delete pService;

		} // if found a match

	} // loop detect, stop and delete

} // onStopZeroConfServices



}	} // namespace SwissalpS::QtSssSapp
