#ifndef VERSIONCHECKER_H
#define VERSIONCHECKER_H

#include <QObject>
#include <QThread>
#include <QQmlListProperty>
#include <QDomDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "requestworker.h"

class VersionChecker : public QObject
{
    Q_OBJECT
public:
    VersionChecker(QObject *parent = 0);
    void getLatest();

public slots:
    void processResult(const QString &value, const QString &updText);

signals:
    void resultProcessed();
    void completed(const QString &value, const QString &updText);

private:
    RequestWorker * worker;
    QThread * thread;
    QString latestVersion;
};

#endif // VERSIONCHECKER_H
