#include "serverhandler.h"
#include <QDebug>
#include "flag.h"

ServerHandler::ServerHandler(QObject *parent) : QObject(parent){
    setState(SHSTATE_DONE);
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

    setState(SHSTATE_PROCESSED);
}

void ServerHandler::getServerInfo(){
    setState(SHSTATE_RETRIEVING);
    netReply.reset(netManager.get(QNetworkRequest(QUrl("http://webzvpn.ru/servers.php"))));
    connect(netReply.get(), SIGNAL(finished()), this, SLOT(processReply()));
}

void ServerHandler::changeState(int newState){
    if (newState == 0)
        setState(SHSTATE_DONE);
    else
        setState(SHSTATE_FAILED);
}

void ServerHandler::processReply(){
    QString msg;
    QByteArray ba = netReply->readAll();
    QString result(ba);
    qDebug() << result.indexOf("==!!!:::", 0);
    result = result.split("==!!!:::")[0];
    qDebug() << result.size();
    result = result.split("!!!:::")[1];
    ba.clear();
    ba.append(result);
    result = QByteArray::fromBase64(ba);

    setupServerList(result);
}

void ServerHandler::setState(SHState newState){
    curState = newState;
    emit stateChanged(curState);
}
