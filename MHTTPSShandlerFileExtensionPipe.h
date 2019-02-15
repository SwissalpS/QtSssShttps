#ifndef SwissalpS_QtSssSapp_MHTTPSSHANDLERFILEEXTENSIONPIPE_H
#define SwissalpS_QtSssSapp_MHTTPSSHANDLERFILEEXTENSIONPIPE_H

#include <QLocalSocket>
#include <QObject>
#include <QProcess>

#include "MHTTPSShandlerFileExtensionBase.h"



namespace SwissalpS { namespace QtSssSapp {



class MHTTPSShFEPexe : public QObject {

	Q_OBJECT

protected:
	bool bHaveReadHeaders;
	WWWSrequest *pRequest;
	WWWSresponse *pResponse;
	QProcess *pProcess;

public:
	explicit MHTTPSShFEPexe(WWWSrequest *pRequest, const QString &sShellScript,
							QStringList &asArguments);
	virtual ~MHTTPSShFEPexe() {}

	virtual void start();
signals:
	void debugMessage(const QString &sMessage) const;
	void respond(WWWSresponse *pResponse) const;

public slots:
	inline virtual void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("MHTTPSShFEPexe:" + sMessage); }

	virtual void onError(QProcess::ProcessError eError);
	virtual void onFinished(int iExitCode, QProcess::ExitStatus eExitStatus);
	virtual void onReadyReadStandardError();
	virtual void onReadyReadStandardOutput();
	virtual void onStarted();

}; // MHTTPSShFEPexe



class MHTTPSShandlerFileExtensionPipe : public MHTTPSShandlerFileExtensionBase {

	Q_OBJECT

private:

protected:
	QString sPathPipe;
	QString sPathWaker;

	virtual void doMatch(WWWSrequest *pRequest) override;

public:
	explicit MHTTPSShandlerFileExtensionPipe(QJsonObject ojoHandlerConf,
									  QObject *pParent = nullptr) :
		MHTTPSShandlerFileExtensionBase(ojoHandlerConf, pParent),
		sPathPipe(""),
		sPathWaker("") {}

	inline virtual ~MHTTPSShandlerFileExtensionPipe() {}

	virtual void init() override;

signals:
	//void debugMessage(const QString &sMessage) const;

public slots:
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("MHTTPSShandlerFileExtensionPipe:" + sMessage); }

	inline void onRespond(WWWSresponse *pResponse) const {
		Q_EMIT this->respond(pResponse); }

}; // MHTTPSShandlerFileExtensionPipe



class MHTTPSShFELSexe : public QObject {

	Q_OBJECT

protected:
	WWWSrequest *pRequest;
	QLocalSocket *pSocket;

public:
	explicit MHTTPSShFELSexe(WWWSrequest *pRequest, const QString &sPathSocket);

	virtual ~MHTTPSShFELSexe() {}

	inline virtual void start() { this->pSocket->connectToServer(); }

signals:
	void respond(MHTTPSShFELSexe *) const;
	void debugMessage(const QString &sMessage) const;

public slots:
	void onConnected();
	inline virtual void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("MHTTPSShFELSexe:" + sMessage); }

	void onDisconnected();
	void onError(QLocalSocket::LocalSocketError eError);
	void onReadyRead();

}; // MHTTPSShFELSexe



class MHTTPSShandlerFileExtensionLocalSocket : public MHTTPSShandlerFileExtensionBase {

	Q_OBJECT

private:

protected:
	QString sPathSocket;
	QString sPathWaker;

	virtual void doMatch(WWWSrequest *pRequest) override;

public:
	explicit MHTTPSShandlerFileExtensionLocalSocket(QJsonObject ojoHandlerConf,
									  QObject *pParent = nullptr) :
		MHTTPSShandlerFileExtensionBase(ojoHandlerConf, pParent),
		sPathSocket(""),
		sPathWaker("") {}

	inline virtual ~MHTTPSShandlerFileExtensionLocalSocket() {}

	virtual void init() override;

signals:

public slots:
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("MHTTPSShandlerFileExtensionLocalSocket:" + sMessage); }

}; // MHTTPSShandlerFileExtensionLocalSocket



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_MHTTPSSHANDLERFILEEXTENSIONPIPE_H
