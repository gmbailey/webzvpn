#include "ovpncontroller.h"
#include "version.h"
#include <QDateTime>

OvpnController::OvpnController(QObject *parent) :
    QObject(parent),
    _state(OVSTATE_DISCONNECTED),
    compressed(true),
    pid(0)
{
    newIp = "127.0.0.1";

    default_dns[0] = "146.185.134.104";
    default_dns[1] = "192.241.172.159";
    setProgramStatus("Disconnected");
    loadSettings();

    checker = new VersionChecker();

    connect(checker, SIGNAL(completed(QString, QString)), this, SLOT(setLatestVersion(QString, QString)));
}

OvpnController::~OvpnController(){

}

QString OvpnController::getLogText() const{
    return logText;
}

QString OvpnController::getUserName() const{
    return userName;
}

QString OvpnController::getUserPass() const{
    return userPass;
}

QString OvpnController::getServer() const{
    return server;
}

QString OvpnController::getServerName() const{
    return serverName;
}

QString OvpnController::getServerLoad() const{
    return serverLoad;
}

unsigned int OvpnController::getPort() const{
    return port;
}

bool OvpnController::getCompressed() const{
    return compressed;
}

QString OvpnController::getCaCertFile() const{
    return caCertFile;
}

QString OvpnController::getConfigFile() const{
    return configFile;
}

QString OvpnController::getIp() const{
    return newIp;
}

void OvpnController::StartTimer(){
    if (NULL != _timer_state.get())
        _timer_state->stop();
    _timer_state.reset(new QTimer(this));
    connect(_timer_state.get(), SIGNAL(timeout()), this, SLOT(timer_CheckState()));
    _timer_state->start(1200);
}

void OvpnController::checkState(){
    if (isOvRunning()){
        if (NULL != soc.get()){
            if (soc->isOpen() && soc->isValid()){
                if (soc->state() == QAbstractSocket::ConnectedState){
                    soc->write("state\n");
                    soc->flush();
                }
            }
        }
        else{
            // first time
            initWatcher();
            attachMgmt();
        }
    }
}

void OvpnController::findOldIp(){
    qDebug() << "findOldIp: started";
    if (_th_oldip.get() == NULL){
        _th_oldip.reset(new Thread_OldIp());
        connect(_th_oldip.get(), &Thread_OldIp::resultReady, this, &OvpnController::processOldIp);
        _th_oldip->start();
    }
}

void OvpnController::processOldIp(QString ip){
    oldIp = ip;
    delete _th_oldip.release();
}

bool OvpnController::getAutoStart() const{
    return autoStart;
}

bool OvpnController::getAutoConnect() const{
    return autoConnect;
}

bool OvpnController::getAutoReconnect() const{
    return autoReconnect;
}

bool OvpnController::getRememberLogin() const{
    return rememberLogin;
}

QString OvpnController::getCurVersion() const{
    return curVersion;
}

QString OvpnController::getUpdateText() const{
    return updateText;
}

QString OvpnController::getProgramStatus() const{
    return programStatus;
}

QString OvpnController::getLatestVersion() const{
    return latestVersion;
}

void OvpnController::launchUpdater(){
    QTimer::singleShot(2000, this, SLOT(launch()));
}

void OvpnController::initConnection(){
    setState(OVSTATE_INITCONN);

    //Check Credentials
    if(userName.isEmpty() || userPass.isEmpty()){
        setState(OVSTATE_NOLOGIN);
        return;
    }

    //Check Server
    if(server.isEmpty()){
        setState(OVSTATE_NOSERVER);
        return;
    }

    startConn();
}

void OvpnController::cancelInitConn(){
    setState(OVSTATE_DISCONNECTED);
}

void OvpnController::startConn(){
//    findOldIp();          Disabled atm

    setProgramStatus("Disconnected");
    setState(OVSTATE_DISCONNECTED);
    stopConn();

    setState(OVSTATE_STARTING);

    try{
//        OsSpecific::Instance()->SetIPv6(false);
#ifdef Q_OS_WIN
        OsSpecific::Instance()->EnableTap();
#endif
    }
    catch(std::exception & ex)
    {
        log::logt(ex.what());
        logAppend(ex.what());

      //  if (IsDisableIPv6())
      //  {
            return;
      //  }
    }

    localAddr = "127.0.0.1";
    localPort = 25340;
    _tunerr = false;
    _err = false;
    processing = false;

    if (NULL != _watcher.get()){
        qDebug() << "NULL != _watcher.get()";
        _watcher->removePath(PathHelper::Instance()->OpenvpnLogPfn());
        delete _watcher.release();
    }

    if (NULL != soc.get()){
        soc->abort();
        soc->disconnect();
        delete soc.release();
    }


    QFile ff(PathHelper::Instance()->OpenvpnConfigPfn());
    if (ff.open(QIODevice::WriteOnly)){
        ff.write("client\n");
        ff.write("dev tun\n");
        ff.write("proto "); ff.write("udp\n");
        ff.write("remote "); ff.write(server.toLatin1()); ff.write(" "); ff.write("50005"); ff.write("\n");
        ff.write("resolv-retry infinite\n");
//        ff.write("cipher AES-256-CBC\n");
//        ff.write("auth SHA512\n");
        ff.write("user nobody\n");
        ff.write("group nobody\n");
        ff.write("nobind\n");
        ff.write("persist-key\n");
        ff.write("persist-tun\n");
        ff.write("verb 3\n");
        ff.write("auth-user-pass\n");
        ff.write("remote-cert-tls server\n");
        ff.write("comp-lzo\n");
        ff.write("reneg-sec 0\n");
        ff.write("route-method exe\n");
        ff.write("route-delay 2\n");
        ff.write("explicit-exit-notify 2");
        ff.write("ping 10");
        ff.write("ping-restart 120");

        ff.flush();
        ff.close();
    }
    else{
        QString se = "Cannot write config file '" + PathHelper::Instance()->OpenvpnConfigPfn() + "'";
        log::logt(se);
        logAppend(se);
        return;
    }

    QStringList args;
    args
        << "--auth-nocache"
        << "--config" << PathHelper::Instance()->OpenvpnConfigPfn() // /tmp/webzvpn.ovpn

#ifndef Q_OS_WIN
        << "--daemon"	// does not work at windows
#endif
        << "--ca" << PathHelper::Instance()->ProxyshCaCert()	// /tmp/webzvpn.crt
        << "--management" << localAddr << QString::number(localPort)
        << "--management-hold"
        << "--management-query-passwords"
        << "--log" << PathHelper::Instance()->OpenvpnLogPfn()			// /tmp/openvpn.log
        << "--script-security" << "3" << "system"

#ifdef Q_OS_MAC			// TODO: -0 DNS on linux
        << "--up" << PathHelper::Instance()->UpScriptPfn()				// /Applications/Webzvpn.app/Contents/Resources/client.up.webzvpn.sh
        << "--down" << PathHelper::Instance()->DownScriptPfn()		// /Applications/Webzvpn.app/Contents/Resources/client.down.webzvpn.sh
#endif
        << "--up-restart"

    ;

//    if (!Setting::Instance()->Dns1().isEmpty())
        args << "--dhcp-option" << "DNS" << default_dns[0];
//    if (!Setting::Instance()->Dns2().isEmpty())
        args << "--dhcp-option" << "DNS" << default_dns[1];

    QString program = PathHelper::Instance()->OpenvpnPathfilename();

    log::logt("Prog is: " + program);
    logAppend("Prog is: " + program);
    QString params = args.join(' ');
    log::logt("Args are:" + params);
    logAppend("Args are:" + params);

    bool ok = false;
    try{
#ifndef Q_OS_WIN
        _paramFile.reset(new QTemporaryFile());
        if (!_paramFile->open())
            throw std::runtime_error("Cannot create tmp file.");

        OsSpecific::Instance()->SetRights();		// lean inside, throw on error
        qDebug() << "in here";
        _paramFile->write(params.toLatin1());
        _paramFile->flush();

        QStringList arg3;
        arg3 << _paramFile->fileName();
        _paramFile->close();

        log::logt("######  before exec ####");
        logAppend("######  before exec ####");
        int r3 = QProcess::execute(PathHelper::Instance()->LauncherPfn(), arg3);	// 30ms block
        log::logt("QProcess::execute() returns " + QString::number(r3));
        logAppend("QProcess::execute() returns " + QString::number(r3));
        log::logt("###############");
        logAppend("###############");
        if (r3 != 0)
            throw std::runtime_error(("Cannot run OpenVPN. Error code is: " + QString::number(r3)).toStdString());
        attachMgmt();
#else
        //process->setReadChannel(QProcess::StandardOutput);
        process.reset(new QProcess());
        connect(process.get(), SIGNAL(error(QProcess::ProcessError)), this, SLOT(connectError(QProcess::ProcessError)));
        connect(process.get(), SIGNAL(started()), this, SLOT(connectStarted()));
        connect(process.get(), SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(stChanged(QProcess::ProcessState)));
        connect(process.get(), SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(connectFinished(int, QProcess::ExitStatus)));
        connect(process.get(), SIGNAL(readyReadStandardError()), this, SLOT(readStdErr()));
        connect(process.get(), SIGNAL(readyReadStandardOutput()), this, SLOT(readStdOut()));

        process->start(program, args);
        process->waitForStarted(2000);
        log::logt("Process ID is: " + QString::number(process->processId()));
        logAppend("Process ID is: " + QString::number(process->processId()));
//        emit logTxtChanged(logText);
#endif
        ok = true;
    }
    catch(std::exception & ex){
        log::logt(ex.what());
        logAppend(ex.what());
        //handle error here
    }
    if (_paramFile.get())
        delete _paramFile.release();

    if(ok)
        StartTimer();
    else
        setState(OVSTATE_DISCONNECTED);

    arguments.clear();
}

void OvpnController::stopConn(){
    if (isOvRunning()){
        setState(OVSTATE_DISCONNECTING);
        if (NULL != soc.get()){
            if (soc->isOpen() && soc->isValid())
            {
                if (soc->state() != QAbstractSocket::ConnectedState){
                    log::logt("Cannot send signal SIGTERM due to disconnected socket");
                    logAppend("Cannot send signal SIGTERM due to disconnected socket");
                }
                else{
                    log::logt("signal SIGTERM");
                    logAppend("signal SIGTERM");
                    //_soc->write("echo all\n");
                    //_soc->flush();

                    soc->write("signal SIGTERM\nexit\n");
                    soc->flush();
                    soc->close();
                }
            }
            QObject * to = soc.release();
            to->deleteLater();
        }

        QThread::msleep(200);
        if (process.get() != NULL)
            delete process.release();
        for (int cn = 0; cn < 8 && isOvRunning(); ++cn)
            QThread::msleep(100);		// just sleep now; without this delay it fails to jump

        if (isOvRunning())
            killRunningOvpn();

        setState(OVSTATE_DISCONNECTED);
    }
}

void OvpnController::cancelConn(const QString &msg){
    logAppend(msg);
    stopConn();
}

bool OvpnController::isOvRunning(){
    bool is = false;

#define AA() do { log::logt("iov: " + QString::number(__LINE__)); logAppend("iov: " + QString::number(__LINE__)); } while(0)
#undef AA
#define AA() {}
AA();
    if (NULL != process.get()){
AA();		if (QProcess::Running == process->state() || QProcess::Starting == process->state()){
AA();			is = true;
        }
        else
        {
AA();
//is = true;		// HACK: -0 locate OpenVpn child process
        }
AA();	}
AA();
    if (!is)
    if (NULL != soc.get()){
AA();		if (soc->isOpen())
{AA();			is = true;
}	}
AA();
    if (!is){
        // lookup child
#ifdef WIN32
           ;        // TODO: -0
#else
AA();		QTemporaryFile file(QDir::tempPath() + "/webzvpn-tmp-XXXXXX.sh");
        QTemporaryFile outf(QDir::tempPath() + "/webzvpn-tmp-XXXXXX.out");
        if (file.open())
        if (outf.open()){
AA();			QString script = OsSpecific::Instance()->IsRunningCmd() + outf.fileName();
            file.write(script.toLatin1());
            file.flush();

            QStringList args;
            args << file.fileName();

AA();			int re = QProcess::execute("/bin/bash", args);
AA();			switch (re){
                case -2: log::logt("IsOvRunning(): -2 the process cannot be started"); logAppend("IsOvRunning(): -2 the process cannot be started");
AA();					break;
                case -1: log::logt("IsOvRunning(): -1 the process crashes"); logAppend("IsOvRunning(): -1 the process crashes");
                    break;
                case 0:{
AA();					QByteArray ba = outf.readAll();
                        QString s2(ba);
                        QString s3 = s2.trimmed();
                        int p = s3.indexOf(' ');
                        QString s4 = s3.mid(0, p);
                        bool converted;
                        _pid = s4.toInt(&converted);
AA();						if (converted){
AA();							if (_pid > 0){
AA();								is = true;
//								AttachMgmt();
AA();							}
                        }
AA();						break;
                }
                case 1: is = false;		// no lines
                    break;
                case 2: is = false;		// grep failure
                    break;
                default:
AA();					log::logt("isOvRunning(): ps-grep return code = " + QString::number(re)); logAppend("isOvRunning(): ps-grep return code = " + QString::number(re));
                    break;
            }
        }
#endif  // else WIN32
    }
//log::logt(QString("isOvRunning() returns ") + QString(is ? "true": "false") );
AA();	return is;
}

void OvpnController::connectError(QProcess::ProcessError error){
/*    printf ("Error: %d\n", error);
    if (process.get() != NULL) {
        QByteArray dataOut = process->readAllStandardOutput();
        QByteArray errorOut = process->readAllStandardError();

        printf ("Output text: %s\n", dataOut.data());
        printf ("Error text: %s\n", errorOut.data());
    }

    // Disconnect
    stopConn();*/
    QString errDesc = QString::number(error);
    switch(error) {
    case 0:
        errDesc += " - The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.";
        break;
    case 1:
        errDesc += " - The process crashed some time after starting successfully.";
        break;
    case 2:
        errDesc += " - The process timed out.";
        break;
    case 3:
        errDesc += " - An error occurred when attempting to read from the process. For example, the process may not be running.";
        break;
    case 4:
        errDesc += " - An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.";
        break;
    case 5:
        errDesc += " - An unknown error occurred.";
        break;
    }

    if (NULL != _timer_state.get()){
        _timer_state->stop();
        delete _timer_state.release();
    }

    log::logt("ProcessError:  " + errDesc);
    logAppend("ProcessError:  " + errDesc);
    setState(OVSTATE_DISCONNECTED);
}

void OvpnController::readData(){
    while (process->canReadLine()) {
        QByteArray read = process->readLine();
        //printf ("Output: %s", read.data());

        logAppend(read);

        if (read.endsWith("Initialization Sequence Completed\n")) {
            setState(OVSTATE_CONNECTED);
        }
    }
}

void OvpnController::readStdErr(){
    log::logt("readStderr(): " + process->readAllStandardError());
    logAppend("readStderr(): " + process->readAllStandardError());
}

void OvpnController::readStdOut(){
    log::logt("readStdout(): " + process->readAllStandardOutput());
    logAppend("readStdout(): " + process->readAllStandardError());
}

void OvpnController::connectStarted(){
    log::logt("connectStarted()");
    logAppend("Connect Started");
}

void OvpnController::connectFinished(int exitCode, QProcess::ExitStatus exitStatus){
   log::logt("Connect Finished: exitCode = " + QString::number(exitCode) + " exitStatus = " +  QString::number(exitStatus));
   logAppend("Connect Finished: exitCode = " + QString::number(exitCode) + " exitStatus = " +  QString::number(exitStatus));
   // OpenVpn crushed or just spawn a child and exit during startup
    if (exitCode != 0){	// TODO: -1 handle open vpn process startup

        // TODO: -1 handle used socket
        // MANAGEMENT: Socket bind failed on local address [AF_INET]127.0.0.1:6842: Address already in use

// TODO: -0 delete or not? it spawns child and exits
//		if (NULL != _process.get())
//			delete _process.release();
        if (!isOvRunning())
            setState(OVSTATE_DISCONNECTED);
    }
    else{
        initWatcher();
        attachMgmt();
    }
}

void OvpnController::soc_err(QAbstractSocket::SocketError er){
    log::logt("Error connecting to OpenVPN managemet socket");
    logAppend("Error connecting to OpenVPN managemet socket");
    if (NULL != soc.get())
        delete soc.release();
}

void OvpnController::soc_readyRead(){
    qDebug() << "in soc_readyRead()";
    if (!soc->canReadLine())
        return;
    QString s = soc->readAll();
    soc->flush();

    QStringList sl = s.split('\n', QString::SkipEmptyParts);
    for (int k = 0; k < sl.size(); ++k)
        processLine(sl[k].trimmed());
}

void OvpnController::stChanged(QProcess::ProcessState st){
    log::logt("State Changed: newState = " + QString::number(st));
    logAppend("State Changed: newState = " + QString::number(st));
    initWatcher();
}

void OvpnController::setLogText(const QString &value){
    logText = value;
    emit logTxtChanged(logText);
}

void OvpnController::logAppend(const QString &text){
    if (!text.isEmpty()) {

        QDateTime now = QDateTime::currentDateTimeUtc();

        QString append = now.toString("yyyy-MM-dd-HH-mm-ss ") + text;
#ifndef NDEBUG
    QMessageLogger(__FILE__, __LINE__, 0).debug() << append;
#endif
        // Ensure we end with a newline
        if (!append.endsWith('\n')) {
            append += '\n';
        }
/*
        // How many lines to add
        int newLines = append.count('\n');
        int currentLines = logText.count('\n');
 //       int removeLines = currentLines + newLines - 24;

        // Remove excess lines from the top
        while (removeLines > 0) {
            int nextLine = logText.indexOf('\n');
            if (nextLine > 0) {
                logText = logText.mid(nextLine + 1);
            }
            removeLines--;
        }
*/
        // Add new lines
        logText.append(append);
  //      logText = append;
  //      qDebug() << append;

        emit logTxtChanged(logText);
    }
}

void OvpnController::logFileChanged(const QString &pfn){
    if (processing || _err)
        return;

    processing = true;

    if (pfn == PathHelper::Instance()->OpenvpnLogPfn()){
        QFile f(pfn);
        QByteArray ba;
        if (f.open(QIODevice::ReadOnly)){		// TODO: -2 ensure non-blocking
            if (lastpos > f.size())		// file was truncated
                lastpos = 0;
            if ((lastpos + 1) < f.size()){
                f.seek(lastpos);
                ba = f.read(f.size() - lastpos);
                lastpos = f.size();
            }
            f.close();
        }
        if (!ba.isEmpty()){
            QString s1(ba);
            QStringList sl = s1.split('\n', QString::SkipEmptyParts);
            for (int k = 0; k < sl.size(); ++k)
                processOvLogLine(sl[k]);
        }
    }
    processing = false;
}

void OvpnController::killRunningOvpn(){
    log::logt(QString("KillRunningOV() enter"));
    logAppend("KillRunningOV() enter");
        if (isOvRunning()){
            attachMgmt();
            if (soc.get())
            for (int count = 0; count < 8 && !soc->isOpen() && !soc->isValid(); ++count)
                QThread::msleep(100);		// HACK: -0 just timeout instead of waiting for connection;

            stopConn();		// soft stop

            if (isOvRunning()){
                // TODO: -0 kill -9 prog

                if (pid > 0){
                    QStringList a;
                    a << "-9" << QString::number(pid);
                    try{
                        OsSpecific::Instance()->ExecAsRoot("/bin/kill", a);
                    }
                    catch(std::exception & ex){
                        log::logt(ex.what());
                        logAppend(ex.what());
                        //handle error here
                    }
                }
            }
        }
        log::logt(QString("KillRunningOV() exit"));
        logAppend("KillRunningOV() exit");
}

void OvpnController::setProgramStatus(const QString &value){
    programStatus = value;
    emit progStatusChanged(programStatus);
}

void OvpnController::setUserName(const QString &value){
    if (userName != value){
        userName = value;
        settingsSetValue("username", value);

        qDebug() << "username set: " << userName;

        emit usernameChanged(userName);
    }
}

void OvpnController::setUserPass(const QString &value){
    if (userPass != value){
        userPass = value;
        settingsSetValue("password", value);

        qDebug() << "password set: " << userPass;

        emit passwordChanged(userPass);
    }
}

void OvpnController::setAutoStart(bool val){
    if (autoStart != val){
        autoStart = val;

        settingsSetValue("autoStart", val);
        qDebug() << "autostart configured: " << val;
        if (autoStart){
            qDebug() << "in here autoStart";
#ifdef Q_OS_WIN
            QSettings regSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
            regSettings.setValue("Webz VPN Client", QGuiApplication::applicationFilePath().replace('/','\\'));

#endif

#ifdef Q_OS_OSX
            QSettings settings("info.plist", QSettings::NativeFormat);
            settings.setValue("RunAtLoad", autoStart);
#endif
        }
        else{
#ifdef Q_OS_WIN
            QSettings regSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
            qDebug() << "in here !autoStart";
            regSettings.remove("Webz VPN Client");
#endif

#ifdef Q_OS_OSX
            settings.remove("RunAtLoad");
#endif
        }

    }
}

void OvpnController::setAutoConnect(bool val){
    if (autoConnect != val){
        autoConnect = val;

        settingsSetValue("autoConnect", val);
        qDebug() << "autoConnect configured: " << val;
    }
}

void OvpnController::setAutoReconnect(bool val){
    if (autoReconnect != val){
        autoReconnect = val;

        settingsSetValue("autoReconnect", val);
        qDebug() << "autoReconnect configured: " << val;
    }
}

void OvpnController::setRememberLogin(bool val){
    if (rememberLogin != val){
        rememberLogin = val;

        settingsSetValue("rememberLogin", val);
        qDebug() << "rememberLogin configured: " << val;
    }
}

void OvpnController::setServer(const QString &value){
    if (value != server) {
        server = value;
        settingsSetValue("server", value);
        emit serverChanged(server);
    }
}

void OvpnController::setServerName(const QString &value){
    serverName = value;
    settingsSetValue("serverName", value);
    emit serverNameChanged(serverName);
}

void OvpnController::setServerLoad(const QString &value){
    serverLoad = value;
    settingsSetValue("serverLoad", value);
    emit serverLoadChanged(serverLoad);

}

void OvpnController::setPort(unsigned int value){
    if (value != port) {
        port = value;
        settingsSetValue("port", value);
        emit portChanged(port);
    }
}

void OvpnController::setCompressed(bool value){
    if (value != compressed) {
        compressed = value;
        settingsSetValue("compressed", value);
        emit compressedChanged(compressed);
    }
}

void OvpnController::setCaCertFile(const QString &value){
    if (value != caCertFile) {
        caCertFile = PathHelper::Instance()->ProxyshCaCert();
        settingsSetValue("caCertFile", value);
        emit caCertFileChanged(caCertFile);
    }
}

void OvpnController::setConfigFile(const QString &value){
    if (value != configFile) {
        configFile = value;
        settingsSetValue("configFile", value);
        emit configFileChanged(configFile);
    }
}

void OvpnController::setIp(const QString &value){
    if (value != newIp){
        newIp = value;
        settingsSetValue("newIp", value);
        emit ipChanged(newIp);
    }
}

void OvpnController::timer_CheckState(){
    checkState();
}

void OvpnController::launch(){
   // QProcess::startDetached("Updater.exe");
   // qApp->quit();

#ifdef Q_OS_WIN
    QString updaterExe = "Updater.exe";
    int result = (int)::ShellExecuteA(0, "runas", updaterExe.toUtf8().constData(), 0, 0, SW_SHOWNORMAL);

    if (result <= 32){
        // error handling
    }
#else
    if (!QProcess::startDetached(exeFileName)){
        // error handling
    }
#endif

    qApp->exit();
}

void OvpnController::setLatestVersion(const QString &value, const QString &updText){
    qDebug() << "setLatestVersion";
    if (latestVersion != value){
        latestVersion = value;
    }
    if (latestVersion == curVersion)
        emit verStateChanged(VERSION_LATEST);
    else{
        updateText = updText;
        emit updateTextChanged(updateText);
        emit verStateChanged(VERSION_OUTDATED);
    }
    emit latestVersionChanged(latestVersion);
}

void OvpnController::checkVersion(){
    checker->getLatest();
    emit verStateChanged(VERSION_CHECKING);
}

void OvpnController::attachMgmt(){
    if (NULL != soc.get()){
        disconnect(soc.get(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(soc_err(QAbstractSocket::SocketError)));
        disconnect(soc.get(), SIGNAL(readyRead()), this, SLOT(soc_readyRead()));
        soc->abort();
    }

    soc.reset(new QTcpSocket());
    connect(soc.get(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(soc_err(QAbstractSocket::SocketError)));
    connect(soc.get(), SIGNAL(readyRead()), this, SLOT(soc_readyRead()));
    soc->connectToHost(localAddr, localPort);
}

void OvpnController::initWatcher(){
    if (_watcher.get() == NULL){
        qDebug() << "watcher == null";
        QFile f(PathHelper::Instance()->OpenvpnLogPfn());
        if (f.exists()){
            qDebug() << "if f.exists()";
            lastpos = 0;		// OpenVpn will truncate

            _watcher.reset(new QFileSystemWatcher());
            _watcher->addPath(PathHelper::Instance()->OpenvpnLogPfn());

            log::logt("Monitoring " + PathHelper::Instance()->OpenvpnLogPfn());
            logAppend("Monitoring " + PathHelper::Instance()->OpenvpnLogPfn());
            connect(_watcher.get(), SIGNAL(fileChanged(const QString &)), this, SLOT(logFileChanged(const QString &)));
        }
    }
}

void OvpnController::processOvLogLine(const QString &s){
    if (s.contains("MANAGEMENT: CMD 'state'", Qt::CaseInsensitive)){
        ;	// skip our commands
    }
    else {
        log::logt("OpenVPNlogfile: " + s);
        logAppend("OpenVPNlogfile: " + s);
        if (s.contains("TCPv4_CLIENT link remote:", Qt::CaseInsensitive)) {
            extractNewIp(s);
        }
        else {
            if (s.contains("Initialization Sequence Completed:", Qt::CaseInsensitive)) {
                gotConnected(s);
            }
            else {
                if (s.contains("Opening utun (connect(AF_SYS_CONTROL)): Operation not permitted", Qt::CaseInsensitive)) {
                    gotTunErr(s);
                }
            }
        }
    }
}

void OvpnController::extractNewIp(const QString &s){
    // Tue May 1 03:50:58 2015 TCPv4_CLIENT link remote: [AF_INET]50.31.252.10:443
    int p0 = s.indexOf("TCPv4_CLIENT link remote");
    p0 = s.indexOf(':', p0 + 1);
    if (p0 > -1){
        int p1 = s.indexOf(':', p0 + 1);
        if (p1 > -1){

            int points[3];
            points[0] = s.indexOf('.', p0);
            int p2 = points[0] - 1;
            for (; p2 > p0; --p2){
                if (!s[p2].isDigit())
                    break;
            }
            QString ip = s.mid(p2 + 1, p1 - p2 - 1);
            setIp(ip);
            qDebug() << "extractNewIp: " << ip;
        }
    }
}

void OvpnController::gotConnected(const QString &s){
    setState(OVSTATE_CONNECTED);
    // extract IP
    //1432176303,CONNECTED,SUCCESS,10.14.0.6,91.219.237.159
    int p = -1;
    for (int k = 0; k < 4; ++k)
        p = s.indexOf(',', p + 1);
    if (p > -1){
        QString ip = s.mid(p + 1);
        setIp(ip);
    }

}

void OvpnController::gotTunErr(const QString &s){
    if (!_tunerr && !_err){
        _tunerr = true;
        _err = true;
        //do something with error message
        stopConn();
    }
}

void OvpnController::processLine(QString s){
    if (s.startsWith('>')){
        // >PASSWORD:Need 'Auth' username/password
        int p = s.indexOf(':');
        if (p > -1){
            QString word = s.mid(1, p - 1);
            processRtWord(word, s);
        }
        else{
            // without :
            // just ignore
        }
    }
    else{	// not starting with >
        QString word;
        if (s[0].isDigit()){
            // 1432065857,GET_CONFIG,,,
            int p[4];
            p[0] = s.indexOf(',', 1);
            p[1] = s.indexOf(',', p[0] + 1);
            if (p[1] > -1){
                word = s.mid(p[0] + 1, p[1] - p[0] - 1);

                // check error
                // 1436967388,CONNECTED,ERROR,10.9.0.6,158.255.211.19
                int p3 = s.indexOf(',', p[1] + 1);
                bool err = false;
                if (p3 > -1){
                    if (p3 > p[1] + 1){
                        QString third = s.mid(p[1] + 1, p3 - p[1] - 1);
                        if (third.contains("ERROR", Qt::CaseInsensitive))
                            err = true;
                    }
                }
                if (!err)
                    processStateWord(word, s);
            }
            else{
                word = s;
                processPlainWord(word, s);
            }
        }
        else{
            int p = s.indexOf(':');
            if (p > -1){
                // SUCCESS: 'Auth' password entered, but not yet verified
                word = s.mid(0, p);
            }
            else{
                word = s;
            }
            processPlainWord(word, s);
        }
    }
}

void OvpnController::processRtWord(const QString &word, const QString &s){
    log::logt(s);
    logAppend(s);
    log::logt("processing RT word '" + word + "'");
    logAppend("processing RT word '" + word + "'");

    if (word.compare("INFO", Qt::CaseInsensitive) == 0) {
        // INFO:
        // just ignore
    } else { if (word.compare("HOLD", Qt::CaseInsensitive) == 0){
        if (s.indexOf("hold release") > -1)
        {
            log::logt("hold off");
            logAppend("hold off");
            soc->write("hold off\n");
            log::logt("hold release");
            logAppend("hold release");
            soc->write("hold release\n");
            soc->flush();
        }
    } else { if (word.compare("PASSWORD", Qt::CaseInsensitive) == 0) {
        if (s.indexOf("Need 'Auth' username/password") > -1){
            qDebug() << "password";
            log::logt("sending vpn login+password");
            logAppend("sending vpn login+password");
            QString u1 = "username \"Auth\" \"" + EscapePsw(userName) + "\"\n";
            QString p1 = "password \"Auth\" \"" + EscapePsw(userPass) + "\"\n";
            qDebug() << "user: " << u1 << " , pass: " << p1;
            soc->write(u1.toLatin1());
            soc->write(p1.toLatin1());
            soc->flush();
        }
        else{
            int p = s.indexOf(':');
            if (s.indexOf("Verification Failed", p, Qt::CaseInsensitive) > -1){
                _err = true;
                // OpenVpn exiting
                showErrMsgAndCleanup(s.mid(p + 1));
            }
        }
    } else { if (word.compare("SUCCES", Qt::CaseInsensitive) == 0) {
        ;
    } else { if (word.compare("FATAL", Qt::CaseInsensitive) == 0) {
        _err = true;
        int p = s.indexOf(':');
        QString msg = s.mid(p + 1);
        this->cancelConn(msg);
    }}}}}
}

void OvpnController::processPlainWord(const QString &word, const QString &s){
    qDebug() << "processPlainWord";
    if (word.compare("END", Qt::CaseInsensitive) == 0) {
        ;
    } else {
        log::logt(s);
        logAppend(s);
        log::logt("processing plain word '" + word + "'");
        logAppend("processing plain word '" + word + "'");
        if (word.compare("SUCCESS", Qt::CaseInsensitive) == 0) {
        ;
    } else { if (word.compare("ERROR", Qt::CaseInsensitive) == 0) {
        ;
    }}}
}

void OvpnController::processStateWord(const QString &word, const QString &s){
    bool isnew = false;
    if (word != _prev_st_word){
        log::logt(s);
        logAppend(s);
        log::logt("processing state word '" + word + "'");
        logAppend("processing state word '" + word + "'");
        isnew = true;
    }
    if (word.compare("CONNECTED", Qt::CaseInsensitive) == 0) {
        if (isnew){
  //          setProgramStatus(word);
            gotConnected(s);
        }
    } else { if (word.compare("CONNECTING", Qt::CaseInsensitive) == 0) {
         setState(OVSTATE_CONNECTING);
 //        setProgramStatus(word);
    } else { if (word.compare("WAIT", Qt::CaseInsensitive) == 0) {
 //        setProgramStatus(word);
         setState(OVSTATE_WAIT);
    } else { if (word.compare("AUTH", Qt::CaseInsensitive) == 0) {
  //       setProgramStatus(word);
         setState(OVSTATE_AUTHORIZING);
    } else { if (word.compare("EXITING", Qt::CaseInsensitive) == 0) {
//         setProgramStatus(word);
                        ;
    } else { if (word.compare("RECONNECTING", Qt::CaseInsensitive) == 0) {
        setState(OVSTATE_RECONNECTING);
//        setProgramStatus(word);
    } else { if (word.compare("AUTH", Qt::CaseInsensitive) == 0) {
        setState(OVSTATE_AUTHORIZING);
//        setProgramStatus(word);
    } else { if (word.compare("GET_CONFIG", Qt::CaseInsensitive) == 0) {
//        setProgramStatus(word);
        setState(OVSTATE_GETCONFIG);
    } else { if (word.compare("ASSIGN_IP", Qt::CaseInsensitive) == 0) {
        setState(OVSTATE_ASSIGNIP);
//        setProgramStatus(word);
    } else { if (word.compare("TCP_CONNECT'", Qt::CaseInsensitive) == 0) {
//        setProgramStatus(word);
        setState(OVSTATE_TCPCONNECT);
    }
    }}}}}}}}}
    _prev_st_word = word;
}

void OvpnController::showErrMsgAndCleanup(QString msg){
    delete soc.release();
    delete process.release();

    logAppend(msg);

    setState(OVSTATE_AUTHFAILED);
}

void OvpnController::setState(OvpnState newState){
    if (_state != newState) {
        _state = newState;
        qDebug() << "setState: " << _state << "  |  " << newState;
        emit stateChanged(newState);
    }
}

void OvpnController::getArgs(){
    //Build Config File
    //Clear args first
/*    arguments.clear();

    addOption("client", true);
    addOption("persist-key", true);
    addOption("persist-tun", true);
    addOption("nobind", true);
    addArg("resolv-retry", "infinite");
    addArg("dev", "tun");
    addArg("verb", "3");
    addArg("proto", "udp");
    addArg("user", "nobody");
    addArg("group", "nobody");
    addArgNonempty("remote", server);
    addArgNonempty("port", QString::number(port));
    addOption("comp-lzo", compressed);
    addArg("auth-user-pass");
*/
    QFile ff(PathHelper::Instance()->OpenvpnConfigPfn());
    if (ff.open(QIODevice::WriteOnly)){
        ff.write("client\n");
        ff.write("dev tun\n");
        ff.write("proto "); ff.write("udp\n");
        ff.write("remote "); ff.write("vnetuk.webzvpn.ru"); ff.write(" "); ff.write("5005"); ff.write("\n");
        ff.write("resolv-retry infinite\n");
//        ff.write("cipher AES-256-CBC\n");
//        ff.write("auth SHA512\n");
        ff.write("user nobody\n");
        ff.write("group nobody\n");
        ff.write("nobind\n");
        ff.write("persist-key\n");
        ff.write("persist-tun\n");
        ff.write("verb 3\n");
        ff.write("auth-user-pass\n");
        ff.write("remote-cert-tls server\n");
        ff.write("comp-lzo\n");
        ff.write("reneg-sec 0\n");
        ff.write("route-method exe\n");
        ff.write("route-delay 2\n");

        ff.flush();
        ff.close();
    }
    else{
        QString se = "Cannot write config file '" + PathHelper::Instance()->OpenvpnConfigPfn() + "'";
        log::logt(se);
        logAppend(se);

        return;
    }

    //Config File Built, build Program Args
    arguments.clear();
    addArgNonempty("config", configFile);

#ifndef Q_OS_WIN
    addArgument("daemon");
#endif

    addArgNonempty("ca", PathHelper::Instance()->ProxyshCaCert());
    addArg("management", localAddr);
    addArg("management-hold");
    addArg("management-query-passwords");
    addArg("log", PathHelper::Instance()->OpenvpnLogPfn());

#ifdef Q_OS_MAC			// TODO: -0 DNS on linux
    addArgument("up", PathHelper::Instance()->UpScriptPfn());			// /Applications/Webzvpn.app/Contents/Resources/client.up.webzvpn.sh
    addArgument("down", PathHelper::Instance()->DownScriptPfn());       // /Applications/Webzvpn.app/Contents/Resources/client.down.webzvpn.sh
#endif

    addArg("up-restart");

    qDebug() << "end of get args";
}

void OvpnController::addArg(QString key, QString value) {
    QString argument;

    argument = "--" + key;
    arguments.append(argument);
    if (!value.isEmpty()) {
        arguments.append(value);
    }
}

void OvpnController::addArgNonempty(QString key, QString value) {
    QString argument;

    if (!value.isEmpty()) {
        argument = "--" + key;
        arguments.append(argument);
        arguments.append(value);
    }
}

void OvpnController::addArg(QString key) {
    QString argument;

    argument = "--" + key;
    arguments.append(argument);
}

void OvpnController::addOption(QString key, bool add) {
    if (add) {
        addArg(key);
    }
}

void OvpnController::addValue(QString key) {
    arguments.append(key);
}

void OvpnController::settingsSetValue(QString key, QString value){
    QSettings settings;

    settings.setValue(key, value);
}

void OvpnController::settingsSetValue(QString key, int value){
    QSettings settings;

    settings.setValue(key, value);
}

void OvpnController::loadSettings(){
    QSettings settings;

    // Read configuration settings

    server = settings.value("server", "").toString();
    port = settings.value("port", 50005).toInt();
    serverLoad = settings.value("serverLoad", "").toString();

    caCertFile = settings.value("caCertFile", "").toString();
    configFile = settings.value("configFile", "").toString();
    rememberLogin = settings.value("rememberLogin", true).toBool();

    if (rememberLogin){
        userPass = settings.value("password", "").toString();
        userName = settings.value("username", "").toString();
    }

    serverName = settings.value("serverName", "").toString();

    autoStart = settings.value("autoStart", false).toBool();
    autoConnect = settings.value("autoConnect", false).toBool();
    autoReconnect = settings.value("autoReconnect", false).toBool();

    curVersion = QString::number(WEBZ_MAJOR) + "." + QString::number(WEBZ_MINOR);
    settings.setValue("version", curVersion);
    latestVersion = curVersion;
}

void OvpnController::setVersState(VersionStatus newState){
    verState = newState;
    emit verStateChanged(verState);
}

