#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QtCore/QCoreApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QCoreApplication::setApplicationName("Browser");
    QCoreApplication::setOrganizationName("Yaron Koresh");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    MainWindow browserWindow;
    browserWindow.show();
    
    return app.exec();
}