#ifndef SwissalpS_QtSssSapp_WWWSREQUEST_H
#define SwissalpS_QtSssSapp_WWWSREQUEST_H

#include <QByteArray>
#include <QObject>
#include <QSslSocket>



namespace SwissalpS { namespace QtSssSapp {



class WWWSsession;



class WWWheader : public QObject {

	Q_OBJECT
	Q_DISABLE_COPY(WWWheader)

private:

protected:
	QString sTitle;
	QString sValue;

public:
	explicit WWWheader(QByteArray &aubRaw, QObject *pParent = nullptr);
	virtual ~WWWheader();

	inline bool isNull() const { return this->sTitle.isEmpty(); }
	inline QString title() const { return this->sTitle; }
	inline QString value() const { return this->sValue; }

}; // WWWheader



class WWWSrequest : public QObject {

	Q_OBJECT
	Q_DISABLE_COPY(WWWSrequest)

private:

protected:
	WWWSsession *pSession;
	QByteArray aubRaw;
	QString sBody;
	QString sMethod;
	QString sRequest;
	QHash<QString, WWWheader *> hspHeaders;

public:
	quint8 ubHandlerIndex;

	explicit WWWSrequest(QByteArray &aubRaw, WWWSsession *pSession);
	virtual ~WWWSrequest();

	virtual QHash<QString, QString> arguments() const;
	virtual inline QString body() const { return this->sBody; }
	virtual inline QHash<QString, WWWheader*> headers() const { return this->hspHeaders; }
	virtual inline bool isGet() const { return 0 == this->sMethod.compare("GET", Qt::CaseInsensitive); }
	virtual inline bool isNull() const { return this->sMethod.isEmpty(); }
	virtual inline bool isPost() const { return 0 == this->sMethod.compare("POST", Qt::CaseInsensitive); }
	virtual inline QString method() const { return this->sMethod; }
	virtual QString path() const;
	virtual inline QByteArray rawRequest() const { return this->aubRaw; }
	virtual inline QString request() const { return this->sRequest; }
	virtual inline void setRequest(const QString &sRequestNew) {
		this->sRequest = sRequestNew; }

	virtual QSslSocket *socket() const;

signals:
	void debugMessage(const QString &sMessage) const;

public slots:
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("HTTPrequest:" + sMessage); }

}; // WWWSrequest



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_WWWSREQUEST_H
