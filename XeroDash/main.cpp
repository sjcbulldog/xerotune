#include "XeroDashWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("ErrorCodeXero");
	QCoreApplication::setOrganizationDomain("www.wilsonvillerobotics.com");
	QCoreApplication::setApplicationName("XeroPlotDashboard");
	QCoreApplication::setApplicationVersion("1.0.0");

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);

	qRegisterMetaType<QVector<int> >("QVector<int>");

	XeroDashWindow w;
	w.show();
	return a.exec();
}
