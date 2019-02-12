#include "MHTTPSShandlerDrop.h"



namespace SwissalpS { namespace QtSssSapp {



void MHTTPSShandlerDrop::handle(WWWSrequest *pRequest) {

	this->onDebugMessage(tr("handle %1 %2")
						 .arg(this->oREsearch.pattern()).arg(this->sREreplace));

	QUrl oRequest(pRequest->target());
	QString sPathOrig = oRequest.path();

	QRegularExpressionMatch oMatch = this->oREsearch.match(sPathOrig);
	if (oMatch.hasMatch()) {

		this->onDebugMessage("OK:dropping");

		Q_EMIT this->ignore(pRequest);

	} else {

		Q_EMIT this->nextHandler(pRequest);

	} // if match found or not

} // MHTTPSShandlerDrop::handle



}	} // namespace SwissalpS::QtSssSapp
