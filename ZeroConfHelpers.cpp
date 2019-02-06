#include "ZeroConfHelpers.h"

#include "ModuleConf.h"



namespace SwissalpS { namespace QtSssSapp {



////////// ZeroConfServiceDescriptor ///////////

ZeroConfServiceDescriptor::ZeroConfServiceDescriptor(const QZeroConfService &oService) :
	QObject(nullptr),
	bIsBrowserInfo(true),
	bOK(true),
	pService(nullptr),
	sModuleUID("") {

	this->ojoService.insert(ModuleConf::sTagZeroConfServiceActive, false);
	this->ojoService.insert(ModuleConf::sTagZeroConfServiceDomain, oService.domain());
	this->ojoService.insert(ModuleConf::sTagZeroConfServiceName, oService.name());
	this->ojoService.insert(ModuleConf::sTagZeroConfServicePort, oService.port());
	this->ojoService.insert(ModuleConf::sTagZeroConfServiceType, oService.type());
	this->ojoService.insert("host", oService.host());
	this->ojoService.insert("ip", oService.ip().toString());
	this->ojoService.insert("ipv6", oService.ipv6().toString());
	this->ojoService.insert("interfaceIndex", int(oService.interfaceIndex()));

	QJsonObject ojoTextRecord;
	QJsonArray ojaTextRecords;
	QMap<QByteArray, QByteArray> hTxt = oService.txt();
	QList<QByteArray> aKeys = hTxt.keys();
	QList<QByteArray> aValues = hTxt.values();
	for (int i = 0; i < aKeys.length(); ++i) {

		ojoTextRecord = QJsonObject();
		ojoTextRecord.insert(ModuleConf::sTagZeroConfTXTrecordLabel,
							 QString(aKeys.at(i)));
		ojoTextRecord.insert(ModuleConf::sTagZeroConfTXTrecordValue,
							 QString(aValues.at(i)));

		ojaTextRecords.append(ojoTextRecord);

	} // loop

	this->ojoService.insert(ModuleConf::sTagZeroConfTXTrecord, ojaTextRecords);

} // construct


ZeroConfServiceDescriptor::ZeroConfServiceDescriptor(const QString &sModuleUID,
													 QJsonObject &ojoServiceDescriptor,
													 QObject *pParent) :
	QObject(pParent),
	bIsBrowserInfo(false),
	bOK(false),
	pService(nullptr),
	sModuleUID(sModuleUID) {

	if (sModuleUID.isEmpty()) return;

	quint16 uiPort = quint16(ojoServiceDescriptor.value(
								 ModuleConf::sTagZeroConfServicePort).toInt(0));
	if (!uiPort) return;
	this->ojoService.insert(ModuleConf::sTagZeroConfServicePort, uiPort);

	QString sType = ojoServiceDescriptor.value(
						ModuleConf::sTagZeroConfServiceType).toString("");
	if (sType.isEmpty()) return;
	this->ojoService.insert(ModuleConf::sTagZeroConfServiceType, sType);

	this->ojoService.insert(ModuleConf::sTagZeroConfServiceActive,
							ojoServiceDescriptor.value(
								ModuleConf::sTagZeroConfServiceActive).toBool(
								false));

	this->ojoService.insert(ModuleConf::sTagZeroConfServiceDomain,
							ojoServiceDescriptor.value(
								ModuleConf::sTagZeroConfServiceDomain).toString(
								"local"));

	this->ojoService.insert(ModuleConf::sTagZeroConfServiceName,
							ojoServiceDescriptor.value(
								ModuleConf::sTagZeroConfServiceName).toString(
								QString(SssS_APP_NAME) + ":" + sModuleUID));

	QJsonArray ojaTXTrecords = ojoServiceDescriptor.value(
								   ModuleConf::sTagZeroConfTXTrecord).toArray();

	QJsonArray ojaTextRecords;
	QJsonObject ojoTXTrecord;
	QJsonObject ojoTextRecord;
	QString sLabel;
	bool bGotAppName = false;
	bool bGotVersion = false;
	for (int i = 0; i < ojaTXTrecords.count(); ++i) {

		ojoTXTrecord = ojaTXTrecords.at(i).toObject();

		sLabel = ojoTXTrecord.value(ModuleConf::sTagZeroConfTXTrecordLabel).toString();
		if (sLabel.isEmpty()) continue;

		if (0 == sLabel.compare("appNameFull")) bGotAppName = true;
		if (0 == sLabel.compare("version")) bGotVersion = true;

		ojoTextRecord = QJsonObject();
		ojoTextRecord.insert(ModuleConf::sTagZeroConfTXTrecordLabel, sLabel);
		ojoTextRecord.insert(ModuleConf::sTagZeroConfTXTrecordValue,
							 ojoTXTrecord.value(
								 ModuleConf::sTagZeroConfTXTrecordValue).toString());

		ojaTextRecords.append(ojoTextRecord);

	} // loop

	ojoTextRecord = QJsonObject();
	ojoTextRecord.insert(ModuleConf::sTagZeroConfTXTrecordLabel, "moduleUID");
	ojoTextRecord.insert(ModuleConf::sTagZeroConfTXTrecordValue, sModuleUID);
	ojaTextRecords.append(ojoTextRecord);

	if (!bGotAppName) {

		ojoTextRecord = QJsonObject();
		ojoTextRecord.insert(ModuleConf::sTagZeroConfTXTrecordLabel, "appNameFull");
		ojoTextRecord.insert(ModuleConf::sTagZeroConfTXTrecordValue, SssS_APP_NAME_FULL);
		ojaTextRecords.append(ojoTextRecord);

	} // if no app name

	if (!bGotVersion) {

		ojoTextRecord = QJsonObject();
		ojoTextRecord.insert(ModuleConf::sTagZeroConfTXTrecordLabel, "version");
		ojoTextRecord.insert(ModuleConf::sTagZeroConfTXTrecordValue, SssS_APP_VERSION_DOTTED);
		ojaTextRecords.append(ojoTextRecord);

	} // if no version

	this->ojoService.insert(ModuleConf::sTagZeroConfTXTrecord, ojaTextRecords);

	this->bOK = true;

} // construct


QString ZeroConfServiceDescriptor::domain() const {

	return this->bOK ? this->ojoService.value(
						   ModuleConf::sTagZeroConfServiceDomain).toString()
					 : "local";

} // domain


QString ZeroConfServiceDescriptor::host() const {

	return (this->bOK && this->bIsBrowserInfo)
			? this->ojoService.value("host").toString() : "";

} // host


quint32 ZeroConfServiceDescriptor::interfaceIndex() const {

	return (this->bOK && this->bIsBrowserInfo)
			? quint32(this->ojoService.value("interfaceIndex").toInt()) : 0u;

} // interfaceIndex


QString ZeroConfServiceDescriptor::ip() const {

	return (this->bOK && this->bIsBrowserInfo)
			? this->ojoService.value("ip").toString() : "";

} // ip


QString ZeroConfServiceDescriptor::ipv6() const {

	return (this->bOK && this->bIsBrowserInfo)
			? this->ojoService.value("ipv6").toString() : "";

} // ipv6


bool ZeroConfServiceDescriptor::isActive() const {

	return this->bOK ? this->ojoService.value(
						   ModuleConf::sTagZeroConfServiceActive).toBool(false)
					 : false;

} // isActive


quint16 ZeroConfServiceDescriptor::port() const {

	return this->bOK ? quint16(this->ojoService.value(
								   ModuleConf::sTagZeroConfServicePort).toInt())
					 : 0u;

} // port


QString ZeroConfServiceDescriptor::serviceName() const {

	return this->bOK ? this->ojoService.value(
						   ModuleConf::sTagZeroConfServiceName).toString()
					 : "null";

} // serviceName


QJsonArray ZeroConfServiceDescriptor::textRecords() const {

	return this->bOK ? this->ojoService.value(
						   ModuleConf::sTagZeroConfTXTrecord).toArray()
					 : QJsonArray();

} // textRecords


QString ZeroConfServiceDescriptor::type() const {

	return this->bOK ? this->ojoService.value(
						   ModuleConf::sTagZeroConfServiceType).toString()
					 : "null";

} // type



//////////// ZeroConfBrowser //////////////



ZeroConfBrowser::ZeroConfBrowser(const QString sType, QObject *pParent) :
	QObject(pParent),
	eProtocol(QAbstractSocket::IPv4Protocol),
	pBrowser(nullptr),
	sType(sType) {

} // construct


ZeroConfBrowser::ZeroConfBrowser(const QString sType,
								 QAbstractSocket::NetworkLayerProtocol eProtocol,
								 QObject *pParent) :
	QObject(pParent),
	eProtocol(eProtocol),
	pBrowser(nullptr),
	sType(sType) {

	if (QAbstractSocket::UnknownNetworkLayerProtocol == eProtocol)
		this->eProtocol = QAbstractSocket::IPv4Protocol;

} // construct(protocol)


void ZeroConfBrowser::init() {

	if (nullptr != this->pBrowser) this->stop();

	this->pBrowser = new QZeroConf(this);

	connect(this->pBrowser, SIGNAL(error(QZeroConf::error_t)),
			this, SLOT(onError(QZeroConf::error_t)));

	connect(this->pBrowser, SIGNAL(serviceAdded(QZeroConfService)),
			this, SLOT(onServiceAdded(QZeroConfService)));

	connect(this->pBrowser, SIGNAL(serviceRemoved(QZeroConfService)),
			this, SLOT(onServiceAdded(QZeroConfService)));

	connect(this->pBrowser, SIGNAL(serviceUpdated(QZeroConfService)),
			this, SLOT(onServiceUpdated(QZeroConfService)));

} // init


void ZeroConfBrowser::start() {

	this->init();

	this->pBrowser->startBrowser(this->sType, this->eProtocol);

} // start


void ZeroConfBrowser::stop() {

	if (nullptr == this->pBrowser) return;

	if (this->pBrowser->browserExists()) this->pBrowser->stopBrowser();
	delete this->pBrowser;
	this->pBrowser = nullptr;

} // stop


void ZeroConfBrowser::onError(QZeroConf::error_t eError) {

	if (QZeroConf::noError == eError) return;

	Q_EMIT this->error();

} // onError


void ZeroConfBrowser::onServiceAdded(QZeroConfService oService) {

	Q_EMIT this->serviceAdded(new ZeroConfServiceDescriptor(oService));

} // onServiceAdded


void ZeroConfBrowser::onServiceUpdated(QZeroConfService oService) {

	Q_EMIT this->serviceUpdated(new ZeroConfServiceDescriptor(oService));

} // onServiceUpdated


void ZeroConfBrowser::onServiceRemoved(QZeroConfService oService) {

	Q_EMIT this->serviceRemoved(new ZeroConfServiceDescriptor(oService));

} // onServiceRemoved



}	} // namespace SwissalpS::QtSssSapp
