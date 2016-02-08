#ifndef SERVERHANDLER_H
#define SERVERHANDLER_H

#include <QObject>
#include <QQmlListProperty>
#include <QDomDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "serverinfo.h"

enum SHState {
    SHSTATE_DONE = 0,

    SHSTATE_RETRIEVING,
    SHSTATE_PROCESSED,
    SHSTATE_FAILED,
    SHSTATE_NUM
};


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
    Q_INVOKABLE void getServerInfo();
    Q_INVOKABLE void changeState(int newState);
public slots:
    void processReply();

    void updateServers() {
        getServerInfo();
    }

signals:
    void stateChanged(int curState);

private:
    QList<ServerInfo *> m_servers;
    QNetworkAccessManager netManager;
    std::auto_ptr<QNetworkReply> netReply;

    SHState curState;
    void setState(SHState newState);

};
#endif // SERVERHANDLER_H
