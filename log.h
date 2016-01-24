#ifndef LOG_H
#define LOG_H

#include <QString>
#include <QObject>

class log//: public QObject
{
//    Q_OBJECT
//    Q_PROPERTY(QString logLine READ getLog NOTIFY logUpdated)

public:
    static void logt(const QString & s);
//    QString getLog() { return logText; }

/*public slots:
    void setLogText(const QString &value);*/

/*signals:
    void logUpdated(QString & s);

private:
    static QString logText;*/
};

#endif // LOG_H
