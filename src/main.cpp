#include <QApplication>
#include <QIcon>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("OfficeExtensionManager");
    app.setApplicationDisplayName("Office Extension & Ribbon Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("KhmerOfficeTools");

    QIcon appIcon(":/assets/logo.png");
    app.setWindowIcon(appIcon);

    MainWindow window;
    window.setWindowIcon(appIcon);
    window.show();

    return app.exec();
}
