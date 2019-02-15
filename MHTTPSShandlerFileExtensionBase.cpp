#include "MHTTPSShandlerFileExtensionBase.h"

#include <QDir>
#include <QFileInfo>



namespace SwissalpS { namespace QtSssSapp {



void MHTTPSShandlerFileExtensionBase::init() {

	MHTTPSShandlerBase::init();
	//MHTTPSShandlerRedirect::init();

	if (!this->bActive) return;

	this->sExtension = this->ojoConfig.value(
						  ModuleConf::sTagMHTTPSSHfileExtension).toString();

	if (!this->sExtension.startsWith(".")) this->sExtension = "." + this->sExtension;

	//this->oREsearch = QRegularExpression("^(.*\\." + this->sExtension.toLower() + ")$");

} // init


void MHTTPSShandlerFileExtensionBase::handle(WWWSrequest *pRequest) {

	this->onDebugMessage(tr("handle >%1<").arg(this->sExtension));

	QString sPathNew = pRequest->targetQ().path();

	// TODO: figure out which is faster/cheaper
	if (sPathNew.endsWith(this->sExtension, Qt::CaseInsensitive)) {
	// or
	//QRegularExpressionMatch oMatch = this->oREsearch.match(sPathNew.toLower());
	//if (oMatch.hasMatch()) {

		this->onDebugMessage(tr("OK:Got file-extension match: %1 : %2")
							 .arg(this->sExtension).arg(sPathNew));

		// is it a file or directory?
		QFileInfo oInfo(sPathNew);
		if (!oInfo.exists()) {

			this->onDebugMessage(tr("KO:Does not exist"));

			Q_EMIT this->respond404(pRequest);
			return;

		} // if does not exist

		if (oInfo.isHidden()) {

			this->onDebugMessage("requesting a hidden file");

			Q_EMIT this->respond404(pRequest);
			return;

		} // if hidden

		if (oInfo.isFile()) {

			if (oInfo.isReadable()) {

				this->onDebugMessage("serving file: " + sPathNew);

				// ok do whatever subclass or controller wants to do
				Q_EMIT this->isMatch(pRequest);
				this->doMatch(pRequest);

			} else {

				// TODO: emit internal server error or at least 404
				this->onDebugMessage("KO:file exists but is not readable: "
									 + sPathNew);
				Q_EMIT this->respond404(pRequest);

			} // if is readable or not

			return;

		} // if file exists

		// well then
		Q_EMIT this->respond404(pRequest);
		return;

	} // if match found

	Q_EMIT this->nextHandler(pRequest);

} // MHTTPSShandlerFileExtensionBase::handle



}	} // namespace SwissalpS::QtSssSapp
