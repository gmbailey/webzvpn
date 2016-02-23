#ifndef REQUESTWORKER_H
#define REQUESTWORKER_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class RequestWorker : public QObject
{
    Q_OBJECT
 public:
     RequestWorker(QObject *parent = 0);

     void requestWork(QString link);

protected:
     void run();

public slots:
     void doWork();
     void requestFinished(QNetworkReply *reply);
     void completed();

signals:
     void done();
     void finished();
     void workRequested();
     void resultReceived(QString);
     void versionReceived(QString);

 private:
     QNetworkAccessManager* manager;
     QUrl url;
};

#endif // REQUESTWORKER_H
