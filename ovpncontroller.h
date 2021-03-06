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
#include "thread_oldip.h"
#include "versionchecker.h"

enum OvpnState {
    OVSTATE_DISCONNECTED = 0,

    OVSTATE_STARTING,
    OVSTATE_AUTHORIZING,
    OVSTATE_AUTHFAILED,
    OVSTATE_CONNECTING,
    OVSTATE_WAIT,
    OVSTATE_RECONNECTING,
    OVSTATE_GETCONFIG,
    OVSTATE_ASSIGNIP,
    OVSTATE_TCPCONNECT,
    OVSTATE_CONNECTED,
    OVSTATE_DISCONNECTING,  //11
    OVSTATE_INITCONN,
    OVSTATE_NOLOGIN,
    OVSTATE_NOSERVER,

    OVSTATE_NUM
};

enum VersionStatus {
    VERSION_LATEST = 0,
    VERSION_CHECKING,
    VERSION_OUTDATED,
    VERSION_NUM
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
    Q_PROPERTY (QString serverName READ getServerName WRITE setServerName NOTIFY serverNameChanged)
    Q_PROPERTY (QString serverLoad READ getServerLoad WRITE setServerLoad NOTIFY serverLoadChanged)
    //application settings
    Q_PROPERTY (bool autoStart READ getAutoStart WRITE setAutoStart)
    Q_PROPERTY (bool autoConnect READ getAutoConnect WRITE setAutoConnect)
    Q_PROPERTY (bool autoReconnect READ getAutoReconnect WRITE setAutoReconnect)
    Q_PROPERTY (bool rememberLogin READ getRememberLogin WRITE setRememberLogin)
    Q_PROPERTY (QString curVersion READ getCurVersion)
    Q_PROPERTY (QString programStatus READ getProgramStatus NOTIFY progStatusChanged)
    Q_PROPERTY (QString latestVersion READ getLatestVersion NOTIFY latestVersionChanged)
    Q_PROPERTY (QString updateText READ getUpdateText NOTIFY updateTextChanged)


public:
    explicit OvpnController(QObject *parent = 0);

    ~OvpnController();

    QString getLogText() const;

    // User Credentials getters
    QString getUserName() const;
    QString getUserPass() const;

    // Configuration getters
    QString getServer() const;
    QString getServerName() const;
    QString getServerLoad() const;

    unsigned int getPort() const;
    bool getCompressed() const;
    QString getCaCertFile () const;
    QString getConfigFile () const;
    QString getIp() const;

    QString defaultDns1() { return default_dns[0]; }
    QString defaultDns2() { return default_dns[1]; }

    void StartTimer();
    void checkState(); // timer calls it

    void findOldIp();
    void processOldIp(QString ip);

    //Application Settings
    bool getAutoStart() const;
    bool getAutoConnect() const;
    bool getAutoReconnect() const;
    bool getRememberLogin() const;

    //Version Info
    QString getCurVersion() const;
    QString getUpdateText() const;

    //Program Status
    QString getProgramStatus() const;

    QString getLatestVersion() const;

    //Launch Updater
    Q_INVOKABLE void launchUpdater();

    //Check UserPass
    Q_INVOKABLE void initConnection();
    Q_INVOKABLE void cancelInitConn();

    Q_INVOKABLE void checkVersion();

signals:
    void stateChanged(int ovState);
    void verStateChanged(int curState);
    void logTxtChanged(QString logTxt);

    void progStatusChanged(QString programStatus);

    // Configuration change signals
    void serverChanged(QString &server);
    void serverNameChanged(QString &serverName);
    void serverLoadChanged(QString &serverLoad);
    void portChanged(unsigned int port);
    void compressedChanged(bool compressed);
    void caCertFileChanged(QString &caCertFile);
    void configFileChanged(QString &configFile);
    void usernameChanged(QString &user);
    void passwordChanged(QString &pass);

    void ipChanged(QString &ip);
    void latestVersionChanged(QString &version);
    void updateTextChanged(QString &updText);

public slots:
    void startConn();
    void stopConn();
    void cancelConn(const QString & msg);
    bool isOvRunning();
    void connectError(QProcess::ProcessError error);
    void readData();
    void readStdErr();
    void readStdOut();
    void connectStarted();

    void connectFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void soc_err(QAbstractSocket::SocketError er);
    void soc_readyRead();
    void stChanged(QProcess::ProcessState st);
    void setLogText(const QString &value);
    void logAppend(const QString &text);
    void logFileChanged(const QString &pfn);
    void killRunningOvpn();

    void setProgramStatus(const QString &value);

    // User Cred Settings
    void setUserName(const QString &value);
    void setUserPass(const QString &value);

    // Application settings
    void setAutoStart(bool val);
    void setAutoConnect(bool val);
    void setAutoReconnect(bool val);
    void setRememberLogin(bool val);

    // Config Settings
    void setServer(const QString &value);
    void setServerName(const QString &value);
    void setServerLoad(const QString &value);

    void setPort(unsigned int value);
    void setCompressed(bool value);
    void setCaCertFile(const QString &value);
    void setConfigFile(const QString &value);
    void setIp(const QString &value);

    void timer_CheckState();

    //Launch Updater
    void launch();

    void setLatestVersion(const QString &value, const QString &updText);

private:
    QStringList arguments;
    QString logText;
    QString programStatus;

    //Config Options
    QString server;
    QString serverName;
    QString serverLoad;
    unsigned int port;
    bool compressed;
    QString caCertFile;
    QString configFile;

    //User Credentials
    QString userPass;
    QString userName;

    //Status
    QString newIp;

    //Application Settings
    bool autoStart;
    bool autoConnect;
    bool autoReconnect;
    bool rememberLogin;

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

    void loadSettings();

    void setVersState(VersionStatus newState);

    std::auto_ptr<QFileSystemWatcher> _watcher;
    std::auto_ptr<QTimer> _timer_state;
    std::auto_ptr<Thread_OldIp> _th_oldip;
    QString default_dns[2];
    QString oldIp;

    QString curVersion;
    QString latestVersion;
    VersionChecker * checker;
    VersionStatus verState;
    QString updateText;
};
#endif // OVPNCONTROLLER_H
