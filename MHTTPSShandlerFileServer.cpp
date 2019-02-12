#include "MHTTPSShandlerFileServer.h"

#include <QDir>
#include <QFileInfo>



namespace SwissalpS { namespace QtSssSapp {



void MHTTPSShandlerFileServer::init() {

	MHTTPSShandlerRedirect::init();

	if (!this->bActive) return;

	this->bListDirs = this->ojoConfig.value(
						  ModuleConf::sTagMHTTPSSHlistDirectories).toBool(false);

	this->sDirectoryIndex = this->ojoConfig.value(
								ModuleConf::sTagMHTTPSSHdirectoryIndex).toString();

} // init


void MHTTPSShandlerFileServer::handle(WWWSrequest *pRequest) {

	this->onDebugMessage(tr("handle %1 %2")
						 .arg(this->oREsearch.pattern()).arg(this->sREreplace));

	QUrl oRequest(pRequest->target());
	QString sPathOrig = oRequest.path();

	// TODO: this sounds cool as to making flexible,
	// but it's not practical. We need to ensure ../../../ hacks are not used
	// unless we decide to have Module handle that, since we proxy there anyway
	// that may be a good point to insert authentication.
	QRegularExpressionMatch oMatch = this->oREsearch.match(sPathOrig);
	if (oMatch.hasMatch()) {

		QString sPathNew = sPathOrig;
		sPathNew.replace(this->oREsearch, this->sREreplace);

		this->onDebugMessage(tr("OK:Rewrote from path: %1 to : %2")
							 .arg(sPathOrig).arg(sPathNew));

		// is it a file or directory?
		QFileInfo oInfo(sPathNew);
		if (!oInfo.exists()) {
			this->onDebugMessage("does not exist");
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

				// ok serve that
				this->onDebugMessage("serving file: " + sPathNew);
				Q_EMIT this->respondFile(pRequest, QDir::cleanPath(sPathNew));

			} else {

				// TODO: emit internal server error or at least 404
				this->onDebugMessage("KO:file exists but is not readable: "
									 + sPathNew);
				Q_EMIT this->respond404(pRequest);

			} // if is readable or not

			return;

		} else if (oInfo.isDir()) {
			this->onDebugMessage("isDir");

			if (this->sDirectoryIndex.length()) {

				QString sPathIndex = sPathNew + "/" + this->sDirectoryIndex;
				this->onDebugMessage("got dirIndex: " + sPathIndex);

				QFileInfo oInfoIndex(sPathIndex);
				if (oInfoIndex.isFile()) {
					this->onDebugMessage("exists");

					if (oInfoIndex.isReadable()) {

						// ok serve that
						this->onDebugMessage("serving file");
						Q_EMIT this->respondFile(pRequest, QDir::cleanPath(sPathIndex));

					} else {

						// TODO: emit internal server error or at least 404
						this->onDebugMessage("index file exists but is not readable.");
						Q_EMIT this->respond404(pRequest);

					} // if readable or not

					return;

				} // if isFile
				this->onDebugMessage("does not exist");

			} // if got a directory index setting

			// do we list directories
			if (this->bListDirs) {
				this->onDebugMessage("list dir: " + sPathNew);
				Q_EMIT this->respondDirectoryList(pRequest, QDir::cleanPath(sPathNew) + "/");
				return;
			} // if list dir

		} // is dir

		// well then
		Q_EMIT this->respond404(pRequest);
		return;

	} // if match found

	Q_EMIT this->nextHandler(pRequest);

} // MHTTPSShandlerFileServer::handle



}	} // namespace SwissalpS::QtSssSapp
