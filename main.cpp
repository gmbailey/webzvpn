#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#ifdef QT_QML_DEBUG
#include <QtQuick>
#else
#include <QtQuick>
#endif

#include "filehandler.h"
#include "serverinfo.h"
#include "serverhandler.h"
#include "ovpncontroller.h"
#include <QDebug>

int main(int argc, char *argv[]){
    // Register class to QML
    qmlRegisterType<FileHandler>("FileHandler", 1, 0, "FileHandler");
    qmlRegisterType<ServerHandler>("Servers", 1,0, "ServerHandler");
    qmlRegisterType<ServerInfo>("Servers", 1,0, "ServerInfo");

#ifdef WIN32
    WORD ver = MAKEWORD(2, 2);
    WSADATA wsa;
    int r0 = WSAStartup(ver, &wsa);
    if (r0 != 0)
    {
        fprintf(stderr, "Cannot init winsock lib");
        return 2;
    }
#endif
    QGuiApplication app(argc, argv);

    app.setOrganizationName("webzvpn");
    app.setOrganizationDomain("www.webzvpn.ru");
    app.setApplicationName("webzvpn");

    QQmlApplicationEngine engine;

    OvpnController * ovpnControl = new OvpnController();
    FileHandler * fileHandler = new FileHandler();
    ServerHandler * servHandler = new ServerHandler();

    engine.rootContext()->setContextProperty("OvpnController", ovpnControl);
    engine.rootContext()->setContextProperty("FileHandler", fileHandler);
    engine.rootContext()->setContextProperty("ServerHandler", servHandler);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QQuickWindow* window = qobject_cast<QQuickWindow*>(engine.rootObjects().at(0));
    window->show();
    return app.exec();
}
