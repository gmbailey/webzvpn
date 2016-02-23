#ifndef UPDATELAUNCHER_H
#define UPDATELAUNCHER_H

#include <QObject>

class UpdateLauncher : public QObject
{
    Q_OBJECT
public:
    explicit UpdateLauncher(QObject *parent = 0);

signals:

public slots:
};

#endif // UPDATELAUNCHER_H