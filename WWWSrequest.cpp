#include "WWWSrequest.h"

#include "WWWSsession.h"



namespace SwissalpS { namespace QtSssSapp {



WWWSrequest::WWWSrequest(QByteArray &aubRaw, WWWSsession *pSession) :
	QObject(pSession),
	pSession(pSession),
	aubRaw(aubRaw),
	sBody(""),
	sMethod(""),
	sTarget(""),
	sTargetOriginal(""),
	uiMaxRequestSize(2000000u),
	bRewritesDone(false),
	ubHandlerIndex(0u) {

	this->hspHeaders.clear();

	int iNextMarker = this->aubRaw.indexOf('\n');
	// type reqURL protoversion
	// POST /p?eut=teu&u=uu HTTP/1.1
	// sMethod sTarget ignored
	QByteArray aubFirstLine = this->aubRaw.mid(0, iNextMarker).trimmed();
	QList<QByteArray> aaubPartsFirstLine = aubFirstLine.split(' ');
	if (2 > aaubPartsFirstLine.count()) return;

	this->sMethod = QString::fromLatin1(aaubPartsFirstLine.first()).toUpper();
	this->sTarget = this->sTargetOriginal =
			QString::fromLatin1(aaubPartsFirstLine.at(1));

	// Discard the first line
	QByteArray aubRest = aubRaw.mid(iNextMarker + 1);

	uint uiFirstBodyLine = 0;
	QList<QByteArray> aaubLines = aubRest.split('\n');
	WWWheader *pHeader;
	QByteArray aubLine;

	for (int i = 0; i < aaubLines.length(); ++i) {

		uiFirstBodyLine++;

		aubLine = aaubLines.at(i);
		pHeader = new WWWheader(aubLine, this);
		if (pHeader->isNull()) {

			delete pHeader;
			break;

		} // if empty line encountered

		this->hspHeaders.insert(pHeader->title(), pHeader);

	} // loop

	// TODO: respect encoding from header
	this->sBody = QString::fromUtf8(aubRest).split('\n').mid(uiFirstBodyLine).join('\n');

} // construct


WWWSrequest::~WWWSrequest() {

} // dealloc


QHash<QString, QString> WWWSrequest::arguments() const {

	// TODO: test and check and url encoding...

	QHash<QString, QString> hssArguments;
	QList<QPair<QString, QString>> aassItems = this->queryQ().queryItems();

	for (int i = 0; i < aassItems.length(); ++i) {

		QPair<QString, QString> assPair = aassItems.at(i);
		hssArguments.insert(assPair.first, assPair.second);

	} // loop

	return hssArguments;

} // arguments


QString WWWSrequest::path() const {

	if (this->isNull()) return QString();

	QUrl oRequest(this->sTarget);
	return oRequest.path();

} // path


QSslSocket *WWWSrequest::socket() const {

	return this->pSession->socket();

} // socket



}	} // namespace SwissalpS::QtSssSapp
