#include "log.h"

#ifndef NDEBUG
#include <QtDebug>
#endif
#include <QFile>
#include <QDateTime>

#include "pathhelper.h"
#include "ovpncontroller.h"

void log::logt(const QString & s)
{
    QDateTime now = QDateTime::currentDateTimeUtc();
    QString s1 = now.toString("yyyy-MM-dd-HH-mm-ss ") + s;
#ifndef NDEBUG
    QMessageLogger(__FILE__, __LINE__, 0).debug() << s1;
#endif
    s1 +=  + "\n";
    QFile ff(PathHelper::Instance()->LogPfn());
    if (ff.open(QIODevice::Append))
    {
        ff.write(s1.toLatin1());
        ff.flush();
        ff.close();
    }

//    logText = s1;

 //   emit logUpdated(logText);
}

/*void log::setLogText(const QString &value){
 //   logText = value;
    emit logUpdated(logText);
}*/



