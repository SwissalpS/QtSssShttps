#ifndef SwissalpS_QtSssSapp_WWWSREQUEST_H
#define SwissalpS_QtSssSapp_WWWSREQUEST_H

#include <QByteArray>
#include <QFile>
#include <QHash>
#include <QObject>
#include <QSslSocket>
#include <QUrl>
#include <QUrlQuery>

#include "WWWheader.h"



namespace SwissalpS { namespace QtSssSapp {



class WWWSsession;



class WWWSrequest : public QObject {

	Q_OBJECT
	Q_DISABLE_COPY(WWWSrequest)

private:

protected:
	WWWSsession *pSession;
	QByteArray aubRaw;
	QString sBody;
	QString sMethod;
	QString sTarget;
	QString sTargetOriginal;
	uint uiMaxRequestSize;
	QHash<QString, WWWheader *> hspHeaders;

public:
	bool bRewritesDone;
	quint8 ubHandlerIndex;

	explicit WWWSrequest(QByteArray &aubRaw, WWWSsession *pSession);
	virtual ~WWWSrequest();

	// queryQ() may be what you are looking for
	virtual QHash<QString, QString> arguments() const;
	virtual inline QString body() const { if (this->isNull()) return QString();
		return this->sBody; }

	// also known as anchor: #aFragment
	virtual inline QString fragment() const {
		return this->targetQ().fragment(); }

	virtual inline QHash<QString, WWWheader*> headers() const {
		return this->hspHeaders; }

	virtual inline bool isGet() const {
		return 0 == this->sMethod.compare("GET", Qt::CaseInsensitive); }

	virtual inline bool isNull() const {
		return this->sMethod.isEmpty(); }

	virtual inline bool isPost() const {
		return 0 == this->sMethod.compare("POST", Qt::CaseInsensitive); }

	inline virtual uint maxRequestSize() const { return this->uiMaxRequestSize; }
	virtual inline QString method() const { return this->sMethod; }
	virtual QString path() const;
	// also known as arguments: ?s=foo&t=bar&z=&y
	virtual inline QString query() const {
		return this->targetQ().query(); }

	// better alternative to arguments()
	virtual inline QUrlQuery queryQ() const {
		return QUrlQuery(this->sTarget); }

	virtual inline QByteArray rawRequest() const { return this->aubRaw; }
	virtual inline WWWSsession *session() { return this->pSession; }
	inline virtual void setMaxRequestSize(const uint uiNewSize) {
		this->uiMaxRequestSize = uiNewSize; }

	virtual inline void setTarget(const QString &sTargetNew) {
		this->sTarget = sTargetNew; }

	virtual inline void setTarget(const QUrl &oTargetNew) {
		this->sTarget = oTargetNew.toString(); }

	virtual QSslSocket *socket() const;
	virtual inline QString target() const {
		return this->sTarget; }

	virtual inline QString targetOriginal() const {
		return this->sTargetOriginal; }

	virtual inline QUrl targetQ() const {
		return QUrl(this->sTarget); }

	virtual inline QUrl targetOriginalQ() const {
		return QUrl(this->sTargetOriginal); }

signals:
	void debugMessage(const QString &sMessage) const;

public slots:
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("HTTPrequest:" + sMessage); }

}; // WWWSrequest



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_WWWSREQUEST_H
