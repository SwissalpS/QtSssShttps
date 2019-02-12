#ifndef SwissalpS_QtSssSapp_WWWSRESPONSE_H
#define SwissalpS_QtSssSapp_WWWSRESPONSE_H

#include <QDateTime>
#include <QFile>
#include <QHash>
#include <QObject>
#include <QTimerEvent>

#include "WWWSrequest.h"



namespace SwissalpS { namespace QtSssSapp {



class WWWSresponse : public QObject {

	Q_OBJECT
	Q_DISABLE_COPY(WWWSresponse)

public:

	enum TransportTypes {
		SendHeadersOnly = 0u,
		SendText = 10u,
		SendTextMultipart = 20u,
		SendFile = 30u
	};
	Q_ENUM(TransportTypes)

private:

protected:
	WWWSrequest *pRequest;
	QString sBody;
	QString sPathFile;
	QFile oFile;
	QHash<QString, WWWheader *> hspHeaders;
	quint16 uiStatusCode;
	TransportTypes eTT;
	int iTimer;
	int iCount;
	int iContentLength; //qint32 soon qint64
	int iBufferSize;

	void timerEvent(QTimerEvent *pEvent) override;

public:
	explicit WWWSresponse(WWWSrequest *pRequestParent = nullptr);
	virtual ~WWWSresponse();

	virtual void addHeader(WWWheader* pHeader);
	virtual void addHeader(const QString &sTitle, const QString sValue = "");
	static QString currentRFC7231date();
	virtual inline void dropHeader(WWWheader* pHeader) {
		this->dropHeader(pHeader->title()); }

	virtual void dropHeader(const QString &sTitle);

	virtual inline QString body() const { return this->sBody; }
	virtual inline QString filePath() { return this->sPathFile; }
	virtual void finalize();
	virtual QString fullHeaderString();
	virtual QString hasHeader(const QString &sTitle) const;
	virtual inline QHash<QString, WWWheader*> headers() const {
		return this->hspHeaders; }

	virtual inline void setBody(const QString &sBodyNew) {
		this->sBody = sBodyNew; }

	// the file is to be open and ready for reading
	virtual inline void setFile(const QString &sFile) { this->sPathFile = sFile; }
	virtual inline void setStatusCode(const quint16 uiCode) {
		this->uiStatusCode = uiCode; }

	virtual void startSending();

	virtual inline quint16 statusCode() const { return this->uiStatusCode; }

	static QString stringForStatusCode(const quint16 uiCode);

signals:
	void debugMessage(const QString &sMessage) const;

public slots:
	inline virtual void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("WWWSresponse:" + sMessage); }

	virtual void onDie();
	virtual void onSequentialFileReadyRead();

}; // WWWSresponse



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_WWWSRESPONSE_H
