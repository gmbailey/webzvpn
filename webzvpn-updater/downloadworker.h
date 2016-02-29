#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class DownloadWorker : public QThread
{
    Q_OBJECT
public:
    DownloadWorker(QObject *parent = 0);

    void run();

public slots:
    void startRequest();
    void requestFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager* manager;
};

#endif // DOWNLOADWORKER_H
