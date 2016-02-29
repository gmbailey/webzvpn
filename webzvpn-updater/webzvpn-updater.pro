#-------------------------------------------------
#
# Project created by QtCreator 2016-02-04T17:06:12
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Updater
TEMPLATE = app

win32: {
        WINSDK_DIR = C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A
        WIN_PWD = $$replace(PWD, /, \\)
        OUT_PWD_WIN = $$replace(OUT_PWD, /, \\)
        QMAKE_POST_LINK = "$$WINSDK_DIR/bin/mt.exe -manifest $$quote($$WIN_PWD\\$$basename(TARGET).manifest) -outputresource:$$quote($$OUT_PWD_WIN\\release\\webzvpn.exe;1)"
        RC_FILE = webzvpn.rc
}

SOURCES += main.cpp\
        mainwindow.cpp \
    downloadworker.cpp

HEADERS  += mainwindow.h \
    downloadworker.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
