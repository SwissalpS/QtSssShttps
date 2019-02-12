#include "WWWheader.h"



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

} // construct(aubRaw, pParent)



}	} // namespace SwissalpS::QtSssSapp
