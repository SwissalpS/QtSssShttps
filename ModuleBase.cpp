#include "ModuleBase.h"

#include <QDateTime>
#include <QFile>
#include <QHostInfo>
#include <QJsonArray>



namespace SwissalpS { namespace QtSssSapp {



ModuleBase::ModuleBase(ModuleConf *pConf, QObject *pParent) :
	QObject(pParent),
	pMC(pConf) {

	qsrand(QDateTime::currentMSecsSinceEpoch());

} // construct


ModuleBase::~ModuleBase() {

	// owned by creator, so we don't delete it
	this->pMC = nullptr;

} // dealloc


// to be overridden by subclass
void ModuleBase::die() {

	this->onDebugMessage("die " + this->moduleUID() + ":" + this->pMC->moduleClass());

	this->stop();

	Q_EMIT this->unregisterZeroConfServiceDescriptors(this->moduleUID());

} // die


// to be overridden by subclass
bool ModuleBase::executeBusCommand(const QString &sCommand,
								 const QJsonObject &ojoMessage) {

	// an empty sender uid is valid -> respond to all modules
	QString sSender = ojoMessage.value("sender").toString("");

	// missing respond command means we can't respond, which is not required at times
	QString sRespondCommand = ojoMessage.value("respondCommand").toString("NULL");
	bool bRespond = (0 != sRespondCommand.compare("NULL"));

	// prepare response object
	QJsonObject ojoResponse;

	if (0 == sCommand.compare("active")) {

		if (!bRespond) return true;

		ojoResponse.insert("result", true);
		ojoResponse.insert("value", this->isActive());

	} else if (0 == sCommand.compare("exposeWithZeroConf")) {

		if (!bRespond) return true;
		ojoResponse.insert("result", true);
		// TODO:
		//ojoResponse.insert("value", this->pMC->exposeWithZeroConf());

	} else if (0 == sCommand.compare("getZeroConfEntries")) {

		if (!bRespond) return true;
		ojoResponse.insert("result", true);
		// TODO: take from pMC
		//ojoResponse.insert("value", this->isActive());

	} else if (0 == sCommand.compare("listCommands")) {

		if (!bRespond) return true;

		QStringList asCommands = this->exposedBusCommands();
		QJsonArray ojaCommands;
		for (int i = 0; i < asCommands.length(); ++i) {

			ojaCommands.append(asCommands.at(i));

		} // loop
		ojoResponse.insert("result", true);
		ojoResponse.insert("value", ojaCommands);

	} else if (0 == sCommand.compare("ping")) {

		if (!bRespond) {
			sRespondCommand = "pong";
			bRespond = true;
		}
		ojoResponse.insert("result", true);
		// TODO: should we add some more info to the response?
		// bus messages are local so time etc is not important

	} else if (0 == sCommand.compare("pong")) {

		return true;

	} else if (0 == sCommand.compare("setActive")) {

		this->pMC->setActive(ojoMessage.value("value").toBool(false));

		if (!bRespond) return true;
		ojoResponse.insert("result", true);

	} else if (0 == sCommand.compare("start")) {

		this->start();

		if (!bRespond) return true;
		ojoResponse.insert("result", true);

	} else if (0 == sCommand.compare("stop")) {

		this->stop();

		if (!bRespond) return true;
		ojoResponse.insert("result", true);

	} else {

		// onBusMessage checks if module provides command
		// so this can only happen if subclass calls
		return false;

		//this->onDebugMessage(tr("KO:Bus-Command not coded: " + sCommand));

	} // switch command

	// in case we missed to do it earlier
	if (!bRespond) return true;

	ojoResponse.insert("sender", this->pMC->moduleUID());
	ojoResponse.insert("command", sRespondCommand);
	ojoResponse.insert("context", ojoMessage.value("context").toString(""));

	Q_EMIT this->busMessage(QStringList() << sSender, ojoResponse);

	return true;

} // executeBusCommand


// to be overridden by subclass
QStringList ModuleBase::exposedBusCommands() const {

	// TODO: setActive needs to go or we need to change the behavior in
	// onBusMessage(), module can be set inactive but not active.
	// guess that's OK. ModuleAppControl's WebSocket will have to provide an out
	return QStringList() << "active" << "exposeWithZeroConf"
						 << "getZeroConfEntries" << "listCommands"
						 << "ping" << "pong" << "setActive" << "start" << "stop";

} // exposedBusCommands


// to be overridden by subclass
void ModuleBase::init() {

	if (!this->isActive()) return;

	this->onDebugMessage("init " + this->moduleUID() + ":" + this->pMC->moduleClass());

} // init


// to be overridden by subclass or not
void ModuleBase::initAuthentication() {

	this->onDebugMessage("initAuthentication");

	// init password if needed
	if (0 == this->pMC->password().compare("-")) {

		static const QString sPool = "abcdefghijklmnopqrstuvwxyz0123456789-._?&%*ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		QString sPass;
		while (12 >= sPass.length()) sPass += sPool.at(qrand() % sPool.length());

		this->pMC->setPassword(sPass);

	} // if need to make a password

	// init black/white list by converting any hostnames to IPs
	// and cleaning out any that don't resolve
	QStringList asListBWraw = this->pMC->listBW();
	this->asListBW.clear();
	QHostAddress oHost;
	for (int i = 0; i < asListBWraw.length(); ++i) {

		oHost = ModuleBase::stringToHostAddress(asListBWraw.at(i));
		if (oHost.isNull()) continue;

		this->asListBW.append(oHost.toString());

	} // loop

} // initAuthentication


void ModuleBase::initSSL() {

	this->onDebugMessage("initSSL");

	this->oCertificate.clear();
	this->oPrivateKey.clear();

	QString sPathCert = this->pMC->pathCert();
	QString sPathKey = this->pMC->pathKey();

	QFile oFileCert(sPathCert);
	QFile oFileKey(sPathKey);

	if (!oFileCert.exists()) {

		this->onDebugMessage(tr("KO:No public-certificate-file found at: ") + sPathCert);

		return;

	} // if no certificate at path

	if (!oFileKey.exists()) {

		this->onDebugMessage(tr("KO:No private-key-file found at: ") + sPathKey);

		return;

	} // if no key at path

	oFileCert.open(QIODevice::ReadOnly);
	oFileKey.open(QIODevice::ReadOnly);

	QSslCertificate oCert(&oFileCert, QSsl::Pem); //QSsl::Der
	QSslKey oKey(&oFileKey, QSsl::Rsa, QSsl::Pem); //  Opaque, Rsa,	Dsa, Ec

	oFileCert.close();
	oFileKey.close();

	this->oCertificate = oCert;
	this->oPrivateKey = oKey;

} // initSSL


// to be overridden by subclass
void ModuleBase::onBusMessage(const QJsonObject &ojoMessage) {

	if (!this->isActive()) return;

	QString sCommand = ojoMessage.value("command").toString("");
	QString sSender = ojoMessage.value("sender").toString("");
	if (sSender.isEmpty()) return;
	if (!this->exposedBusCommands().contains(sCommand)) return;

	this->onDebugMessage("onBusMessage");

	this->executeBusCommand(sCommand, ojoMessage);

} // onBusMessage


// to be overridden by subclass
void ModuleBase::start() {

	if (!this->isActive()) return;

	this->onDebugMessage("start " + this->moduleUID() + ":" + this->pMC->moduleClass());

	QJsonArray ojaZC = this->pMC->zeroConf();
	QJsonObject ojoZC;
	for (int j = 0; j < ojaZC.count(); ++j) {

		ojoZC = ojaZC.at(j);
		Q_EMIT this->registerZeroConfServiceDescriptor(
					new ZeroConfServiceDescriptor(this->moduleUID(), ojoZC));

	} // loop each service

	Q_EMIT this->startZeroConfServices(this->moduleUID());

} // start


// to be overridden by subclass
void ModuleBase::stop() {

	this->onDebugMessage("stop " + this->moduleUID() + ":" + this->pMC->moduleClass());

	Q_EMIT this->stopZeroConfServices(this->moduleUID());

} // stop


QHostAddress ModuleBase::stringToHostAddress(const QString &sHostOrIP) {

	QHostAddress oHost(sHostOrIP);
	if (QAbstractSocket::UnknownNetworkLayerProtocol == oHost.protocol()) {

		// is not an IP, probably hostname. Let's look up IP
		QHostInfo oHostInfo = QHostInfo::fromName(sHostOrIP);
		QList<QHostAddress> aoIPs = oHostInfo.addresses();
		oHost = QHostAddress();
		if (aoIPs.isEmpty()) {

			//this->onDebugMessage("KO:Can not resolve IP/Host: " + sHostOrIP);
			return oHost;

		} // if could not resolve IP

		for (int i = 0; i < aoIPs.length(); ++i) {

			oHost = aoIPs.at(i);
			if (QAbstractSocket::IPv4Protocol == oHost.protocol()) break;

		} // loop

	} // if not an IP in config

	return oHost;

} // stringToHostAddress



}	} // namespace SwissalpS::QtSssSapp
