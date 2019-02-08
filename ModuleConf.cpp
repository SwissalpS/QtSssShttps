#include "ModuleConf.h"

#include <QDateTime>
#include <QJsonArray>



namespace SwissalpS { namespace QtSssSapp {


const QString ModuleConf::sModuleAppControl = QStringLiteral("ModuleAppControl");
const QString ModuleConf::sModuleBase = QStringLiteral("ModuleBase");
//const QString ModuleConf::sModule = QStringLiteral("ModuleEncryptedWebSocketServer");
const QString ModuleConf::sModuleHTTPSserver = QStringLiteral("ModuleHTTPSserver");
//const QString ModuleConf::sModule = QStringLiteral("ModuleHTTPSfileServer");
//const QString ModuleConf::sModule = QStringLiteral("ModuleHTTPSnodeJSserver");
//const QString ModuleConf::sModule = QStringLiteral("ModuleHTTPSphpServer");
//const QString ModuleConf::sModule = QStringLiteral("ModuleHTTPSrubyServer");
//const QString ModuleConf::sModule = QStringLiteral("ModuleMediaServer");
//const QString ModuleConf::sModule = QStringLiteral("ModuleMediaPlayer");
//const QString ModuleConf::sModule = QStringLiteral("ModuleNibblersServer");
//const QString ModuleConf::sModule = QStringLiteral("ModuleNibblersPlayer");
//const QString ModuleConf::sModule = QStringLiteral("ModuleShellRunner"); // no arguments, only run scripts in base-dir
const QString ModuleConf::sModuleZeroConfig = QStringLiteral("ModuleZeroConfig");

const QString ModuleConf::sTagActive = QStringLiteral("bActive");
const QString ModuleConf::sTagClass = QStringLiteral("sClass");
const QString ModuleConf::sTagListenIP = QStringLiteral("sListenIPorHostname");
const QString ModuleConf::sTagListenPort = QStringLiteral("uiListenPort");
const QString ModuleConf::sTagListenPort1 = QStringLiteral("uiListenPort1");
const QString ModuleConf::sTagListenPort2 = QStringLiteral("uiListenPort2");
const QString ModuleConf::sTagListIP = QStringLiteral("aListIP");
const QString ModuleConf::sTagListIPisBlackNotWhite = QStringLiteral("bListIPisBlackNotWhite");
const QString ModuleConf::sTagMaxConnections = QStringLiteral("uiMaxConnections");
const QString ModuleConf::sTagPassword = QStringLiteral("sPassword");
const QString ModuleConf::sTagPathCert = QStringLiteral("sPathCert");
const QString ModuleConf::sTagPathKey = QStringLiteral("sPathKey");
const QString ModuleConf::sTagTargetIP = QStringLiteral("sTargetIPorHostname");
const QString ModuleConf::sTagTargetPort = QStringLiteral("uiTargetPort");
const QString ModuleConf::sTagTargetPort1 = QStringLiteral("uiTargetPort1");
const QString ModuleConf::sTagTargetPort2 = QStringLiteral("uiTargetPort2");
const QString ModuleConf::sTagUID = QStringLiteral("sUID");
const QString ModuleConf::sTagUsername = QStringLiteral("sUser");
const QString ModuleConf::sTagZeroConf = QStringLiteral("aZeroConf");
const QString ModuleConf::sTagZeroConfServiceActive = QStringLiteral("bPublish");
const QString ModuleConf::sTagZeroConfServiceDomain = QStringLiteral("sDomain");
const QString ModuleConf::sTagZeroConfServiceName = QStringLiteral("sServiceName");
const QString ModuleConf::sTagZeroConfServicePort = QStringLiteral("uiPort");
const QString ModuleConf::sTagZeroConfServiceType = QStringLiteral("sType");
const QString ModuleConf::sTagZeroConfTXTrecord = QStringLiteral("aTextRecords");
const QString ModuleConf::sTagZeroConfTXTrecordLabel = QStringLiteral("sLabel");
const QString ModuleConf::sTagZeroConfTXTrecordValue = QStringLiteral("sValue");


ModuleConf::ModuleConf(const QString sPath, QObject *pParent) :
	PersistantObject(sPath, pParent) {

	if (!this->isOK()) return;

} // construct


ModuleConf::~ModuleConf() {

} // dealloc


bool ModuleConf::isActive() const {

	return this->toJSONobject().value(sTagActive).toBool(false);

} // isActive


bool ModuleConf::isListBlack() const {

	return this->toJSONobject().value(sTagListIPisBlackNotWhite).toBool(false);

} // isListBlack


QStringList ModuleConf::listBW() const {

	QJsonArray oList = this->toJSONobject().value(sTagListIP).toArray();
	QStringList asList;

	for (int i = 0; i < oList.count(); ++i) {

		asList << oList.at(i).toString();

	} // loop

	return  asList;

} // listBW


QString ModuleConf::localIP() const {

	return this->toJSONobject().value(sTagListenIP).toString("0.0.0.0");

} // localIP


quint16 ModuleConf::localPort() const {

	return this->toJSONobject().value(sTagListenPort).toInt(8080);

} // localPort


quint16 ModuleConf::localPort1() const {

	return this->toJSONobject().value(sTagListenPort1).toInt(8081);

} // localPort1


quint16 ModuleConf::localPort2() const {

	return this->toJSONobject().value(sTagListenPort2).toInt(8082);

} // localPort2


quint16 ModuleConf::maxConnections() const {

	return this->toJSONobject().value(sTagMaxConnections).toInt(25);

} // maxConnections


QString ModuleConf::moduleClass() const {

	return this->toJSONobject().value(sTagClass).toString("Unknown");

} // moduleClass


QString ModuleConf::moduleUID() const {

	return this->toJSONobject().value(sTagUID).toString("Unnamed");

} // moduleUID


QString ModuleConf::password() const {

	return this->toJSONobject().value(sTagPassword).toString("-");

} // password


QString ModuleConf::pathCert() const {

	return this->toJSONobject().value(sTagPathCert).toString("");

} // pathCert


QString ModuleConf::pathKey() const {

	return this->toJSONobject().value(sTagPathKey).toString("");

} // pathKey


void ModuleConf::setActive(const bool bActive) {

	// check if changed
	if (this->isActive() == bActive) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagActive, bActive);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setActive


void ModuleConf::setListBlackOrWhite(const bool bBlackNotWhite) {

	// check if changed
	if (this->isListBlack() == bBlackNotWhite) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagListIPisBlackNotWhite, bBlackNotWhite);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setListBlackOrWhite


void ModuleConf::setListBW(const QStringList &asList) {

	QJsonObject oJo = this->toJSONobject();

	QJsonArray oList;

	for (int i = 0; i < asList.length(); ++i) {

		oList << asList.at(i);

	} // loop

	oJo.insert(sTagListIP, oList);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setListBW


void ModuleConf::setLocalIP(const QString &sHost) {

	// check if changed
	if (0 == this->localIP().compare(sHost)) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagListenIP, sHost);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setLocalIP


void ModuleConf::setLocalPort(const quint16 uiPort) {

	// check if changed
	if (this->localPort() == uiPort) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagListenPort, uiPort);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setLocalPort


void ModuleConf::setLocalPort1(const quint16 uiPort) {

	// check if changed
	if (this->localPort1() == uiPort) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagListenPort1, uiPort);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setLocalPort1


void ModuleConf::setLocalPort2(const quint16 uiPort) {

	// check if changed
	if (this->localPort2() == uiPort) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagListenPort2, uiPort);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setLocalPort2


void ModuleConf::setMaxConnections(const quint16 uiMax) {

	// check if changed
	if (this->maxConnections() == uiMax) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagMaxConnections, uiMax);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setMaxConnections


void ModuleConf::setModuleClass(const QString &sClass) {

	// check if changed
	if (0 == this->moduleClass().compare(sClass)) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagClass, sClass);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setModuleClass


void ModuleConf::setModuleUID(const QString &sUID) {

	// check if changed
	if (0 == this->moduleUID().compare(sUID)) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagUID, sUID);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setModuleUID


void ModuleConf::setPassword(const QString &sPassword) {

	// check if changed
	if (0 == this->password().compare(sPassword)) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagPassword, sPassword);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setPassword


void ModuleConf::setPathCert(const QString &sPathCert) {

	// check if changed
	if (0 == this->pathCert().compare(sPathCert)) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagPathCert, sPathCert);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setPathCert


void ModuleConf::setPathKey(const QString &sPathKey) {

	// check if changed
	if (0 == this->pathKey().compare(sPathKey)) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagPathKey, sPathKey);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setPathKey


void ModuleConf::setTargetIP(const QString &sHost) {

	// check if changed
	if (0 == this->targetIP().compare(sHost)) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagTargetIP, sHost);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setTargetIP


void ModuleConf::setTargetPort(const quint16 uiPort) {

	// check if changed
	if (this->targetPort() == uiPort) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagTargetPort, uiPort);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setTargetPort


void ModuleConf::setTargetPort1(const quint16 uiPort) {

	// check if changed
	if (this->targetPort1() == uiPort) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagTargetPort1, uiPort);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setTargetPort1


void ModuleConf::setTargetPort2(const quint16 uiPort) {

	// check if changed
	if (this->targetPort2() == uiPort) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagTargetPort2, uiPort);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setTargetPort2


void ModuleConf::setUser(const QString &sUser) {

	// check if changed
	if (0 == this->user().compare(sUser)) return;

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagUsername, sUser);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setUser


void ModuleConf::setZeroConf(const QJsonArray &ojaServiceDescriptors) {

	QJsonObject oJo = this->toJSONobject();

	oJo.insert(sTagZeroConf, ojaServiceDescriptors);
	this->oJdoc.setObject(oJo);

	this->bChanged = true;
	this->save();

} // setZeroConf


QString ModuleConf::targetIP() const {

	return this->toJSONobject().value(sTagTargetIP).toString("255.255.255.255");

} // targetIP


quint16 ModuleConf::targetPort() const {

	return this->toJSONobject().value(sTagTargetPort).toInt(8080);

} // targetPort


quint16 ModuleConf::targetPort1() const {

	return this->toJSONobject().value(sTagTargetPort1).toInt(8081);

} // targetPort1


quint16 ModuleConf::targetPort2() const {

	return this->toJSONobject().value(sTagTargetPort2).toInt(8082);

} // targetPort2


QString ModuleConf::user() const {

	return this->toJSONobject().value(sTagUsername).toString("admin");

} // user


QJsonArray ModuleConf::zeroConf() const {

	return this->toJSONobject().value(sTagZeroConf).toArray();

} // zeroConf



}	} // namespace SwissalpS::QtSssSapp
