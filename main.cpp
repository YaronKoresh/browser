#include "mainwindow.h"
#include "constants.h"

#include <QtWidgets/QApplication>
#include <QtCore/QCoreApplication>
#include <QtCore/QLoggingCategory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName(AppConstants::APP_NAME);
    QCoreApplication::setOrganizationName(AppConstants::APP_ORGANIZATION);
    QCoreApplication::setApplicationVersion(AppConstants::APP_VERSION);

    QLoggingCategory::setFilterRules(QStringLiteral("browser.*.debug=false"));

    MainWindow browserWindow;
    browserWindow.show();

    return app.exec();
}
