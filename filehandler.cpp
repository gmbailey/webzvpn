#include "filehandler.h"
#include <QDebug>
FileHandler::FileHandler(){

}

void FileHandler::save(QString data, QString fileName){
    QFile file(fileName);

    if(file.open(QIODevice::ReadWrite)){
        QTextStream stream(&file);
        stream << data;
    }

    return;
}

QString FileHandler::read(QString fileName){
    QFile file(fileName);
    QString data;
        if ( file.open(QIODevice::ReadOnly) ) {
            QString line;
            QTextStream t( &file );
            do {
                line = t.readLine();
                data += line;
             } while (!line.isNull());

            file.close();
        } else {

            return QString();
        }
        return data;
}

FileHandler::~FileHandler(){

}
