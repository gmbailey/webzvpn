#ifndef SERVERHANDLER_H
#define SERVERHANDLER_H

#include <QObject>
#include <QQmlListProperty>
#include <QDomDocument>
#include "serverinfo.h"

class ServerHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<ServerInfo> servers READ servers)

public:
    ServerHandler(QObject *parent = 0);
    QQmlListProperty<ServerInfo> servers();
    int serverCount() const;
    ServerInfo *server(int index) const;
Q_INVOKABLE void setupServerList(const QString &value);

private:
    QList<ServerInfo *> m_servers;
};
#endif // SERVERHANDLER_H
