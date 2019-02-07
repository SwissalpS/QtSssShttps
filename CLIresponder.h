#ifndef SwissalpS_QtSssSapp_CLIRESPONDER_H
#define SwissalpS_QtSssSapp_CLIRESPONDER_H

#include <QObject>
#include <QSocketNotifier>
#include <QTextStream>



namespace SwissalpS { namespace QtSssSapp {



class AppController;
class AppSettings;
class CLIresponder;



class CLIpage : public QObject {

	Q_OBJECT

private:

protected:
	AppController *pAC;
	AppSettings *pAS;
	CLIresponder *pCLI;

public:
	explicit CLIpage(AppSettings *pSettings, AppController *pAppController,
					 CLIresponder *pParent);

	virtual ~CLIpage() {}

	static void print(const QString &sMessage);

signals:
	void debugMessage(const QString &sMessage) const;
	void done() const;

public slots:
	inline virtual void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("CLIpage:" + sMessage); }

	inline virtual void onLine(const QString &sLine) {
		Q_UNUSED(sLine)
	} // onLine

	inline virtual void start() {}

}; // CLIpage



class CLIpageWelcome : public CLIpage {

	Q_OBJECT

private:

public:
	explicit CLIpageWelcome(AppSettings *pSettings, AppController *pAppController,
					 CLIresponder *pParent);

	virtual ~CLIpageWelcome() {}

signals:

public slots:
	inline virtual void onLine(const QString &sMessage) override {
		Q_UNUSED(sMessage)
		Q_EMIT this->done(); }

	virtual void start() override;

}; // CLIpageWelcome



class CLIpageMainMenu : public CLIpage {

Q_OBJECT

private:
	quint8 ubStep;

public:
	explicit CLIpageMainMenu(AppSettings *pSettings, AppController *pAppController,
				 CLIresponder *pParent);

virtual ~CLIpageMainMenu() {}

signals:

public slots:
virtual void onLine(const QString &sMessage) override;

virtual void start() override;

}; // CLIpageMainMenu


class CLIresponder : public QObject {

	Q_OBJECT

private:
	AppController *pAC;
	AppSettings *pAS;
	QTextStream *pTS;
	int iTimer;
	QSocketNotifier oNotifier;
	CLIpage *pPageCurrent;
	CLIpageMainMenu *pPageMainMenu;
	CLIpageWelcome *pPageWelcome;

protected:
	bool menuAddModule();
	bool menuEditModule();
	bool menuMain();

protected slots:
	void timerEvent(QTimerEvent *pEvent);

public:
	explicit CLIresponder(AppSettings *pSettings, AppController *pParent = nullptr);
	virtual ~CLIresponder();

	static void print(const QString &sMessage);

signals:
	void debugMessage(const QString &sMessage) const;

public slots:
	virtual void onActivated(int iSocket);
	inline void onDebugMessage(const QString &sMessage) const {
		Q_EMIT this->debugMessage("CLIresponder:" + sMessage); }

	virtual void onPageDone();
	virtual void start();

}; // CLIresponder



}	} // namespace SwissalpS::QtSssSapp



#endif // SwissalpS_QtSssSapp_CLIRESPONDER_H
