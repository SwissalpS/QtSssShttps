#include "MHTTPSShandlerRedirect.h"



namespace SwissalpS { namespace QtSssSapp {



void MHTTPSShandlerRedirect::handle(WWWSrequest *pRequest) {

	this->onDebugMessage(tr("handle %1 %2")
						 .arg(this->oREsearch.pattern()).arg(this->sREreplace));

	QUrl oRequest(pRequest->target());
	QString sPathOrig = oRequest.path();

	QRegularExpressionMatch oMatch = this->oREsearch.match(sPathOrig);
	if (oMatch.hasMatch()) {

		QString sPathNew = sPathOrig;
		sPathNew.replace(this->oREsearch, this->sREreplace);

		this->onDebugMessage(tr("OK:Rewrote from path: %1 to url: %2")
							 .arg(sPathOrig).arg(sPathNew));

		Q_EMIT this->respond301(pRequest, sPathNew);

		return;

	} // if match found

	Q_EMIT this->nextHandler(pRequest);

} // MHTTPSShandlerRedirect::handle



void MHTTPSShandlerRedirect::init() {

	if (!this->bActive) return;

	MHTTPSShandlerBase::init();

	QString sSearch = this->ojoConfig.value("sSearch").toString();
	QString sReplace = this->ojoConfig.value("sReplace").toString();

	this->oREsearch = QRegularExpression(sSearch);

	if (!this->oREsearch.isValid()) {

		this->onDebugMessage("KO:Invalid regular expression found: "
							 + sSearch);

		this->bActive = false;

		return;

	} // if invalid regular expression given

	this->sREreplace = sReplace;

} // init



}	} // namespace SwissalpS::QtSssSapp
