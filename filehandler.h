#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QObject>
#include <QFile>
#include <QTextStream>

class FileHandler : public QObject
{
    Q_OBJECT

public:
    FileHandler();
    Q_INVOKABLE void save(QString data, QString fileName);
    Q_INVOKABLE QString read(QString fileName);
    ~FileHandler();
};

#endif // FILEHANDLER_H
