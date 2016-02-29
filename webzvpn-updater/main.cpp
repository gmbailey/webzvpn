#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    a.setOrganizationName("webzvpn");
    a.setOrganizationDomain("www.webzvpn.ru");
    a.setApplicationName("Webz VPN Client");
    w.show();

    return a.exec();
}
