#include "serverhandler.h"
#include <QDebug>
#include "flag.h"

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

            serv->setName(loc.text());

            serv->setLoad(load.text());

            QString fl = flag::IconFromSrvName(loc.text());
            qDebug() << "Flag : " << fl;

            serv->setFlag(fl);
            m_servers.push_back(serv);
        }
    }
}
