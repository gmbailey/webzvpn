#ifndef OVPNCONTROLLER_H
#define OVPNCONTROLLER_H

#include <QObject>
#include <memory>
#include <QTcpSocket>
#include <QProcess>
#include <QSettings>
#include <QFileSystemWatcher>
#include <QTemporaryFile>
#include <QFile>
#include <QDir>
#include <stdexcept>
#include "pathhelper.h"
#include <QThread>
#include "log.h"
#include "osspecific.h"
#include "common.h"
#include <QTimer>

enum OvpnState {
    OVSTATE_DISCONNECTED = 0,

    OVSTATE_CONNECTING,
    OVSTATE_CONNECTED,
    OVSTATE_DISCONNECTING,

    OVSTATE_NUM
};


class OvpnController : public QObject
{
    Q_OBJECT

    Q_PROPERTY (QString logText READ getLogText WRITE setLogText NOTIFY logTxtChanged)
    Q_PROPERTY (QString userName READ getUserName WRITE setUserName NOTIFY usernameChanged)
    Q_PROPERTY (QString userPass READ getUserPass WRITE setUserPass NOTIFY passwordChanged)
    Q_PROPERTY (QString server READ getServer WRITE setServer NOTIFY serverChanged)
    Q_PROPERTY (unsigned int port READ getPort WRITE setPort NOTIFY portChanged)
    Q_PROPERTY (bool compressed READ getCompressed WRITE setCompressed NOTIFY compressedChanged)
    Q_PROPERTY (QString caCertFile READ getCaCertFile WRITE setCaCertFile NOTIFY caCertFileChanged)
    Q_PROPERTY (QString configFile READ getConfigFile WRITE setConfigFile NOTIFY configFileChanged)
    Q_PROPERTY (QString newIp READ getIp WRITE setIp NOTIFY ipChanged)

public:
    explicit OvpnController(QObject *parent = 0);

    QString getLogText() const;

    // User Credentials getters
    QString getUserName() const;
    QString getUserPass() const;

    // Configuration getters
    QString getServer() const;
    unsigned int getPort() const;
    bool getCompressed() const;
    QString getCaCertFile () const;
    QString getConfigFile () const;
    QString getIp() const;

    QString defaultDns1() { return default_dns[0]; }
    QString defaultDns2() { return default_dns[1]; }

    void StartTimer();
    void checkState(); // timer calls it


signals:
    void stateChanged(int state);
    void logTxtChanged(QString & logTxt);

    // Configuration change signals
    void serverChanged(QString &server);
    void portChanged(unsigned int port);
    void compressedChanged(bool compressed);
    void caCertFileChanged(QString &caCertFile);
    void configFileChanged(QString &configFile);
    void usernameChanged(QString &user);
    void passwordChanged(QString &pass);

    void ipChanged(QString &ip);

public slots:
    void startConn();
    void stopConn();
    bool isOvRunning();
    void readError(QProcess::ProcessError error);
    void readData();
    void readStdErr();
    void readStdOut();
    void connectStarted();

    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void soc_err(QAbstractSocket::SocketError er);
    void soc_readyRead();
    void stChanged(QProcess::ProcessState st);
    void setLogText(const QString &value);
    void logAppend(const QString &text);
    void logFileChanged(const QString &pfn);
    void killRunningOvpn();

    // User Cred Settings
    void setUserName(const QString &value);
    void setUserPass(const QString &value);

    // Config Settings
    void setServer(const QString &value);
    void setPort(unsigned int value);
    void setCompressed(bool value);
    void setCaCertFile(const QString &value);
    void setConfigFile(const QString &value);
    void setIp(const QString &value);

    void timer_CheckState();

private:
    QStringList arguments;
    QString logText;

    //Config Options
    QString server;
    unsigned int port;
    bool compressed;
    QString caCertFile;
    QString configFile;

    //User Credentials
    QString userPass;
    QString userName;

    //Status
    QString newIp;

    std::auto_ptr<QTemporaryFile> _paramFile;
    std::auto_ptr<QProcess> process;
    int pid;
    void attachMgmt();
    std::auto_ptr<QTcpSocket> soc;
    QString localAddr;
    quint16 localPort;
    qint64 lastpos;
    void initWatcher();
    bool processing;
    void processOvLogLine(const QString & s);
    void extractNewIp(const QString & s);
    void gotConnected(const QString & s);
    void gotTunErr(const QString & s);
    bool _tunerr;
    bool _err;
    void processLine(QString s);
    void processRtWord(const QString & word, const QString & s);
    void processPlainWord(const QString & word, const QString & s);
    void processStateWord(const QString & word, const QString & s);
    QString _prev_st_word;
    void showErrMsgAndCleanup(QString msg);

    OvpnState _state;
    void setState(OvpnState newState);

    void getArgs();
    void addArg(QString key, QString value);
    void addArg(QString key);
    void addArgNonempty (QString key, QString value);
    void addOption (QString key, bool add);
    void addValue (QString key);
    void settingsSetValue (QString key, QString value);
    void settingsSetValue (QString key, int value);

    std::auto_ptr<QFileSystemWatcher> _watcher;
    std::auto_ptr<QTimer> _timer_state;
    QString default_dns[2];
};
#endif // OVPNCONTROLLER_H
