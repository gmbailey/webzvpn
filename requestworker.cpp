#include "requestworker.h"

#include <QDebug>

RequestWorker::RequestWorker(QObject *parent) : QObject(parent) {//QThread(parent){
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
}

void RequestWorker::requestWork(QString link){
    qDebug() << "RequestWorker: requestWork";
    url = link;
    emit workRequested();
}

void RequestWorker::doWork(){
    qDebug() << "RequestWorker: doWork";

    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);
}

void RequestWorker::requestFinished(QNetworkReply *reply){
    qDebug() << "RequestWorker: requestFinished";
    if (reply->error()) {
        qDebug() << "Error occurred";
    }

    QByteArray ba = reply->readAll();
    QString result(ba);

    if (url.toString() == "https://webzvpn.ru/servers.php"){
        qDebug() << result.indexOf("=!!!:::", 0) << " _ indexOf(=!!!:::)";
        result = result.split("=!!!:::")[0];
        qDebug() << result.size();
        result = result.split("!!!:::")[1];
        ba.clear();
        ba.append(result);
        result = QByteArray::fromBase64(ba);

        reply->deleteLater();

        emit resultReceived(result);
    }

    else if (url.toString() == "https://webzvpn.ru/update.php"){
        result = result.split("::")[0];
        qDebug() << result;

        reply->deleteLater();

        emit versionReceived(result);
    }
}

void RequestWorker::completed(){
    emit done();
    emit finished();
}
