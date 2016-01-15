#include "serverhandler.h"
#include <QDebug>

ServerHandler::ServerHandler(QObject *parent) : QObject(parent){

}

QQmlListProperty<ServerInfo> ServerHandler::servers(){
    return QQmlListProperty<ServerInfo>(this, m_servers);
}

int ServerHandler::serverCount() const{
    return m_servers.count();
}

ServerInfo *ServerHandler::server(int index) const{
    return m_servers.at(index);
}

void ServerHandler::setupServerList(const QString &value){
    QDomDocument doc;
    doc.setContent(value);

    QDomNodeList nl = doc.elementsByTagName("data");
    if (nl.size() > 0){
        for (int k = 0; k < nl.size(); ++k){
            QDomNode se = nl.item(k);
            QDomElement adr = se.firstChildElement("ip");
            QDomElement loc = se.firstChildElement("name");
            QDomElement load = se.firstChildElement("load");
            if (adr.isNull() || loc.isNull() || load.isNull())
                continue;
            ServerInfo *serv = new ServerInfo();
            serv->setAddress(adr.text());
            qDebug() << serv->address();
            serv->setName(loc.text());
            qDebug() << serv->name();
            serv->setLoad(load.text());
            qDebug() << serv->load();
            m_servers.push_back(serv);
        }
    }
}
