#include "versionchecker.h"

VersionChecker::VersionChecker(QObject *parent) : QObject(parent){
    thread = new QThread();
    worker = new RequestWorker();

    worker->moveToThread(thread);
    connect(worker, SIGNAL(workRequested()), thread, SLOT(start()));
    connect(thread, SIGNAL(started()), worker, SLOT(doWork()));
    connect(worker, SIGNAL(versionReceived(QString)), this, SLOT(processResult(QString)));
    connect(this, SIGNAL(resultProcessed()), worker, SLOT(completed()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()), Qt::DirectConnection);
}

void VersionChecker::getLatest(){
    worker->requestWork("https://webzvpn.ru/update.php");
}

void VersionChecker::processResult(const QString &value){
    qDebug() << latestVersion;
    latestVersion = value;
    emit completed(latestVersion);
}
