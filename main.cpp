#include "AppController.h"
#ifdef SssS_USE_GUI
#include <QApplication>
#else
#include <QCoreApplication>
#endif

#include <QTimer>

int main(int iArgumentCount, char *aArguments[]) {

#ifdef SssS_USE_GUI
	QApplication oApp(iArgumentCount, aArguments);
#else
	QCoreApplication oApp(iArgumentCount, aArguments);
#endif

	QCoreApplication::setOrganizationName("SwissalpS");
	QCoreApplication::setOrganizationDomain("SwissalpS.ws");
	QCoreApplication::setApplicationName(SssS_APP_NAME);
	QCoreApplication::setApplicationVersion(SssS_APP_VERSION_DOTTED);

	SwissalpS::QtSssSapp::AppController *pAC =
			SwissalpS::QtSssSapp::AppController::pAppController();

	// This will cause the application to exit when
	// the task signals done.
	QObject::connect(pAC, SIGNAL(done()), &oApp, SLOT(quit()));

	// This will run the app-controller from the application event loop.
	// This guarantees that all classes are prepared and instances created.
	QTimer::singleShot(0, pAC, SLOT(run()));

	return oApp.exec();

} // main
