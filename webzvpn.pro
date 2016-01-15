TEMPLATE = app

QT += network xml core gui
QT += qml quick
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = webzvpn
TEMPLATE = app

macx: {
        TARGET = Webzvpn
        QMAKE_INFO_PLIST = ./Info.plist
        QMAKE_LFLAGS += -F /System/Library/Frameworks/
        LIBS += -framework Security
    target.path = /Applications
    resources.path = /Applications/Webzvpn.app/Contents/Resources
    resources.files = ./resources/*
    INSTALLS = target resources
        ICON = Webzvpn.icns
}

win32: {
        WINSDK_DIR = C:/Program Files/Microsoft SDKs/Windows/v7.1A
        WIN_PWD = $$replace(PWD, /, \\)
        OUT_PWD_WIN = $$replace(OUT_PWD, /, \\)
        QMAKE_POST_LINK = "$$WINSDK_DIR/bin/mt.exe -manifest $$quote($$WIN_PWD\\$$basename(TARGET).manifest) -outputresource:$$quote($$OUT_PWD_WIN\\release\\webzvpn.exe;1)"
        RC_FILE = webzvpn.rc
}

SOURCES += main.cpp \
    osspecific.cpp \
    log.cpp \
    pathhelper.cpp \
    common.cpp \
    ovpncontroller.cpp \
    filehandler.cpp \
    serverhandler.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    osspecific.h \
    log.h \
    pathhelper.h \
    common.h \
    filehandler.h \
    serverhandler.h \
    serverinfo.h \
    ovpncontroller.h

DISTFILES +=