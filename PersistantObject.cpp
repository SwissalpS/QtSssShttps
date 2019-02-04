#include "PersistantObject.h"

#include <QFile>



namespace SwissalpS { namespace QtSssSapp {



PersistantObject::PersistantObject(const QString sPath, QObject *pParent) :
	QObject(pParent),
	bChanged(false),
	bOK(false),
	sPathJSON(sPath) {

	QJsonObject oJo;

	QFile oFile(this->sPathJSON);
	if (oFile.open(QIODevice::ReadOnly)) {

		QByteArray aubData = oFile.readAll();
		oFile.close();

		this->oJdoc = QJsonDocument::fromJson(aubData);

	} else {

		// file does not exist (or we have a permission issue)
		this->oJdoc = QJsonDocument();
		//oJo.insert("sDummy", "dummy");
		this->oJdoc.setObject(oJo);
		this->bChanged = true;

		// if we can't save, we really have an issue
		if (!this->save()) return;

		if (oFile.open(QIODevice::ReadOnly)) {

			QByteArray aubData = oFile.readAll();
			oFile.close();

			this->oJdoc = QJsonDocument::fromJson(aubData);

		} else {

			return;

		} // if able to read file now

	} // if able to read file

	// we really need an object
	if (!this->oJdoc.isObject()) return;

	this->bOK = true;

} // construct


PersistantObject::~PersistantObject() {

} // dealloc


bool PersistantObject::save() {

	if (!this->bChanged) return true;

	QByteArray oData = this->toJSON();

	QFile oFile(this->sPathJSON);
	if (oFile.open(QIODevice::WriteOnly)) {

		oFile.write(oData);
		oFile.close();

	} else {

		this->onDebugMessage(tr("Can NOT save to: ") + this->sPathJSON);

		return false;

	} // if can open for writing

	this->bChanged = false;

	return true;

} // save



}	} // namespace SwissalpS::QtSssSapp
