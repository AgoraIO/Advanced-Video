#include <QApplication>
#include <QDebug>
#include "mainwindow.h"
int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    app.setOrganizationName("Agora");
    app.setOrganizationDomain("agora.io");
    MainWindow w;
    w.show();
    return app.exec();

}

