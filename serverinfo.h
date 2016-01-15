#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <QObject>

class ServerInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString address READ address WRITE setAddress)
    Q_PROPERTY(QString load READ load WRITE setLoad)

public:
    ServerInfo(QObject *parent = 0) : QObject(parent) {}

    QString name() const { return servName; }
    void setName(const QString &nm) { servName = nm; }

    QString address() const { return servAddress; }
    void setAddress(const QString &ad) { servAddress = ad; }

    QString load() const { return servLoad; }
    void setLoad(const QString &ld) { servLoad = ld; }

private:
    QString servName;
    QString servAddress;
    QString servLoad;
};
#endif // SERVERINFO_H
