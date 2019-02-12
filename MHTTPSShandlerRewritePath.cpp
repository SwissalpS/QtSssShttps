#include "MHTTPSShandlerRewritePath.h"



namespace SwissalpS { namespace QtSssSapp {



void MHTTPSShandlerRewritePath::handle(WWWSrequest *pRequest) {

	this->onDebugMessage(tr("handle %1 %2")
						 .arg(this->oREsearch.pattern()).arg(this->sREreplace));

	QUrl oRequest(pRequest->target());
	QString sPathOrig = oRequest.path();

	QRegularExpressionMatch oMatch = this->oREsearch.match(sPathOrig);
	if (oMatch.hasMatch()) {

		if (this->bMatchIsFinal) pRequest->bRewritesDone = true;

		QString sPathNew = sPathOrig;
		sPathNew.replace(this->oREsearch, this->sREreplace);

		this->onDebugMessage(tr("OK:Rewrote path %1 to %2").arg(sPathOrig).arg(sPathNew));

		oRequest.setPath(sPathNew);
		pRequest->setTarget(oRequest.toString());

	} // if match found

	Q_EMIT this->nextHandler(pRequest);

} // MHTTPSShandlerRewritePath::handle


void MHTTPSShandlerRewritePath::init() {

	MHTTPSShandlerRedirect::init();

	if (!this->bActive) return;

	this->bMatchIsFinal = this->ojoConfig.value(
							  "bLeaveRewritesOnMatch").toBool(false);

} // init



}	} // namespace SwissalpS::QtSssSapp
