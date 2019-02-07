#include "CLIresponder.h"

#include <iostream>
#include <QTimerEvent>
#include <unistd.h> // for STDIN_FILENO

#include "AppController.h"
#include "AppSettings.h"



namespace SwissalpS { namespace QtSssSapp {



CLIpage::CLIpage(AppSettings *pSettings, AppController *pAppController, CLIresponder *pParent) :
	QObject(pParent),
	pAC(pAppController),
	pAS(pSettings),
	pCLI(pParent) {

} // construct


void CLIpage::print(const QString &sMessage) {

	CLIresponder::print(sMessage);

} // print



CLIpageMainMenu::CLIpageMainMenu(AppSettings *pSettings,
								 AppController *pAppController,
								 CLIresponder *pParent) :
	CLIpage(pSettings, pAppController, pParent),
	ubStep(0u) {}


void CLIpageMainMenu::onLine(const QString &sMessage) {

	QString sLine = sMessage.trimmed().toLower();

	if (0u == this->ubStep) {

		if (0 == sLine.compare("a")) {

			print("you selected option a");
			//this->ubStep = 1u;

		} else if (0 == sLine.compare("e")) {

			print("you selected option e");
			//this->ubStep = 2u;

		} else if (0 == sLine.compare("q")) {

			print("\n\n Good-bye, have a nice day :-{)\n");
			this->pAC->quit();

		} else {

			print("Invalid input\n\n");

		} // switch command

		Q_EMIT this->done();

	} else if (1u == this->ubStep) {

		//this->pPageAdd->onLine(sMessage);

	} else if (2u == this->ubStep) {

		//this->pPageEdit->onLine(sMessage);

	} // switch sub-menu

} // CLIpageMainMenu::onLine


void CLIpageMainMenu::start() {

	print("Main Menu");
	print("a) Add a module-configuration");
	print("e) Edit a module-configuration");
	print("q) Quit Application");
	print("\nYour choice:");

} // CLIpageMainMenu::start


CLIpageWelcome::CLIpageWelcome(AppSettings *pSettings,
							   AppController *pAppController,
							   CLIresponder *pParent) :
	CLIpage(pSettings, pAppController, pParent) {}


void CLIpageWelcome::start() {

	print("Welcome :-)");

	QVector<ModuleConf *> apConfs = this->pAS->moduleConfs();
	if (apConfs.length())
		print("Found " + QString::number(apConfs.count())
			  + " module-configurations.");
	else
		print("Did not find any module-configurations.");

	QStringList asErrors = this->pAC->moduleLoadErrors();
	if (asErrors.length()) {

		print("Had at least one error/warning loading modules:");
		for (int i = 0; i < asErrors.length(); ++i) {

			this->print(QString::number(i) + ": " + asErrors.at(i));

		} // loop

		print("check that there aren't any missing or surplus commas.");
		print("JSON-files are fickle that way.");

	} // if had errors

	print("\nPress Return to continue...");

} // CLIpageWelcome::start



CLIresponder::CLIresponder(AppSettings *pSettings, AppController *pParent) :
	QObject(pParent),
	pAC(pParent),
	pAS(pSettings),
	pTS(nullptr),
	iTimer(0),
	oNotifier(STDIN_FILENO, QSocketNotifier::Read) {

	this->pTS = new QTextStream(stdin, QIODevice::ReadOnly);

	connect(&this->oNotifier, SIGNAL(activated(int)),
			this, SLOT(onActivated(int)));

	//this->iTimer = startTimer(3000);

} // construct


CLIresponder::~CLIresponder() {

	killTimer(this->iTimer);

	this->pAC = nullptr;
	this->pAS = nullptr;
	if (this->pTS) delete this->pTS;
	this->pTS = nullptr;

} // dealloc


void CLIresponder::onActivated(int iSocket) {
	Q_UNUSED(iSocket)

	QString sLine = this->pTS->readLine();

	if (this->pPageCurrent) this->pPageCurrent->onLine(sLine);

	print(sLine);

} // onActivated


void CLIresponder::onPageDone() {

	if (this->pPageCurrent == this->pPageWelcome) {

		this->pPageCurrent = this->pPageMainMenu;

	} else if (this->pPageCurrent == this->pPageMainMenu) {


	}

	this->pPageCurrent->start();

} // onPageDone


void CLIresponder::print(const QString &sMessage) {

	std::cout << sMessage.toStdString();
	std::cout << std::endl;

} // print


void CLIresponder::start() {

	this->pPageMainMenu = new CLIpageMainMenu(this->pAS, this->pAC, this);

	connect(this->pPageMainMenu, SIGNAL(done()),
			this, SLOT(onPageDone()));

	this->pPageCurrent = this->pPageWelcome = new CLIpageWelcome(this->pAS,
																 this->pAC,
																 this);

	connect(this->pPageCurrent, SIGNAL(done()),
			this, SLOT(onPageDone()));

	this->pPageCurrent->start();

} // start


void CLIresponder::timerEvent(QTimerEvent *pEvent) {

	print("timer event");

	pEvent->accept();

} // timerEvent



}	} // namespace SwissalpS::QtSssSapp
