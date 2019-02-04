#include "WWWSrequest.h"

#include "WWWSsession.h"



namespace SwissalpS { namespace QtSssSapp {



WWWheader::WWWheader(QByteArray &aubRaw, QObject *pParent) :
	QObject(pParent),
	sTitle(""),
	sValue("") {

	QByteArray aubLine = aubRaw.trimmed();

	if (aubLine.isEmpty()) return;

	int iNextMarker = aubLine.indexOf(':');
	if (0 > iNextMarker) return;

	this->sTitle = QString::fromLatin1(aubLine.left(iNextMarker).trimmed());
	this->sValue = QString::fromLatin1(aubLine.mid(iNextMarker + 1).trimmed());

} // construct


WWWheader::~WWWheader() {

} // dealloc



WWWSrequest::WWWSrequest(QByteArray &aubRaw, WWWSsession *pSession) :
	QObject(pSession),
	pSession(pSession),
	aubRaw(aubRaw),
	sBody(""),
	sMethod(""),
	sRequest("") {

	this->hspHeaders.clear();

	int iNextMarker = this->aubRaw.indexOf('\n');
	// type reqURL protoversion
	// POST /p?eut=teu&u=uu HTTP/1.1
	// sMethod sRequest ignored
	QByteArray aubFirstLine = this->aubRaw.mid(0, iNextMarker).trimmed();
	QList<QByteArray> aaubPartsFirstLine = aubFirstLine.split(' ');
	if (2 > aaubPartsFirstLine.count()) return;

	this->sMethod = QString::fromLatin1(aaubPartsFirstLine.first());
	this->sRequest = QString::fromLatin1(aaubPartsFirstLine.at(1));

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
	int iPos = this->sRequest.indexOf('?');

	if (0 > iPos) return hssArguments;

	QString sArgument;
	QString sArguments = this->sRequest.mid(iPos + 1);
	QStringList asArgument;
	QStringList asArguments = sArguments.split('&');
	for (int i = 0; i < asArguments.length(); ++i) {

		sArgument = asArguments.at(i);
		asArgument = sArgument.split('=');

		if (1 == asArgument.length()) {

			hssArguments.insert(asArgument.first(), "");

		} else {

			hssArguments.insert(asArgument.first(), asArgument.at(1));

		} // if has pair or not

	} // loop

	return hssArguments;

} // arguments


QString WWWSrequest::path() const {

	if (this->isNull()) return QString();

	int iPos = this->sRequest.indexOf('?');

	if (0 >= iPos) return this->sRequest;

	return this->sRequest.left(iPos);

} // path


QSslSocket *WWWSrequest::socket() const {

	return this->pSession->socket();

} // socket



}	} // namespace SwissalpS::QtSssSapp
