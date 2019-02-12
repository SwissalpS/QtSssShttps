#include "MHTTPSShandlerBase.h"



namespace SwissalpS { namespace QtSssSapp {



MHTTPSShandlerBase::MHTTPSShandlerBase(QJsonObject ojoHandlerConf, QObject *pParent) :
	QObject(pParent),
	pDelegate(nullptr),
	ojoConfig(ojoHandlerConf),
	sPathBase(""),
	bActive(false) {

	this->bActive = this->ojoConfig.value(ModuleConf::sTagActive).toBool(false);

} // construct


QStringList MHTTPSShandlerBase::allGenerllyUsedHTTPmethods() {

	static QStringList asAllGenerllyUsedHTTPmethods;

	if (asAllGenerllyUsedHTTPmethods.isEmpty()) {

		asAllGenerllyUsedHTTPmethods << QStringLiteral("GET")
									 << QStringLiteral("HEAD")
									 << QStringLiteral("POST")
									 << QStringLiteral("PUT")
									 << QStringLiteral("DELETE")
									 << QStringLiteral("CONNECT")
									 << QStringLiteral("OPTIONS")
									 << QStringLiteral("TRACE")
									 << QStringLiteral("PATCH");

	} // if first run

	return asAllGenerllyUsedHTTPmethods;

} // allGenerllyUsedHTTPmethods


void MHTTPSShandlerBase::handle(WWWSrequest *pRequest) {

	if (this->bActive) {

		if (nullptr != this->pDelegate) {

			// TODO: check if delegate has matching signature
			if (this->pDelegate->handle(pRequest)) return;

		} // if got a delegate

	} // if am active at all

	Q_EMIT this->nextHandler(pRequest);

} // MHTTPSShandlerBase::handle


void MHTTPSShandlerBase::init() {

	if (!this->bActive) return;

	this->sPathBase = this->ojoConfig.value("sDirRoot").toString("");

	this->initMethods();

} // init


void MHTTPSShandlerBase::initMethods() {

	if (!this->bActive) return;

	QJsonArray ojaMethods = this->ojoConfig.value("aMethods").toArray();
	QStringList asAllMethods = MHTTPSShandlerBase::allGenerllyUsedHTTPmethods();

	int iTotal = ojaMethods.count();
	bool bAllAdded = false;
	this->asMethods.clear();
	QStringList asValidNotMethods;
	QStringList asValidMethods;
	QString sMethod;
	if (iTotal) {

		for (int i = 0; i < iTotal; ++i) {

			sMethod = ojaMethods.at(i).toString().toUpper();
			// check for *
			if (0 == sMethod.compare("*")) {
				bAllAdded = true;
				continue;
			} // if got *

			// check for not-methods
			if (sMethod.startsWith("^")) {
				if (asAllMethods.contains(sMethod.mid(1)))
					asValidNotMethods.append(sMethod);
				continue;
			} // if not-method

			if (bAllAdded) continue;

			if (asAllMethods.contains(sMethod))
				asValidMethods.append(sMethod);

		} // loop preparing lists

	} else bAllAdded = true;

	if (bAllAdded) asValidMethods.append(asAllMethods);
	this->asMethods = asValidNotMethods;

	for (int i = 0; i < asValidMethods.length(); ++i) {

		sMethod = asValidMethods.at(i);
		if (this->asMethods.contains(sMethod)) continue;
		if (this->asMethods.contains("^" + sMethod)) continue;
		this->asMethods.append(sMethod);

	} // loop into definitive list

	this->onDebugMessage(tr("OK:Added %1 HTTP-methods of %2 given.")
						 .arg(this->asMethods.length()).arg(iTotal));

	// deactivate if no methods....should not be needed
	if (0 == this->asMethods.length()) {
		this->onDebugMessage(tr("KO:No HTTP-methods -> deactivating this handler."));
		this->bActive = false;
	} // if no methods at all

} // initMethods


void MHTTPSShandlerBase::setDelegate(MHTTPSShandlerDelegateBase *pDelegate) {

	if (this->pDelegate) disconnect(this->pDelegate);

	this->pDelegate = pDelegate;

	connect(this->pDelegate, SIGNAL(debugMessage(QString)),
			this, SLOT(onDebugMessage(QString)));

} // setDelegate



}	} // namespace SwissalpS::QtSssSapp
