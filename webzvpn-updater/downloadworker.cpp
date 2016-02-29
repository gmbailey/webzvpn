#include "downloadworker.h"
#include <QDebug>

DownloadWorker::DownloadWorker(QObject *parent) : QThread(parent) {
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
}

void DownloadWorker::run(){
    startRequest();
}

void DownloadWorker::startRequest(){
    QUrl url("http://webzvpn.ru/servers.php");
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);
}

void DownloadWorker::requestFinished(QNetworkReply *reply){
    QUrl url = reply->url();
    if (reply->error()) {
        qDebug() << "Error occurred";
    }

    reply->deleteLater();

    this->exit();
}
