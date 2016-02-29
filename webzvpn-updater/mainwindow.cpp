#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifdef Q_OS_WIN
    #include <windows.h>
    #include <shellapi.h>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);

    this->setMaximumHeight(270);
    this->setMaximumWidth(230);
    this->setMinimumHeight(270);
    this->setMinimumWidth(230);
    this->statusBar()->setSizeGripEnabled(false);
    ui->mainToolBar->setHidden(true);

    movie = new QMovie(":/loading.gif");
    if (!movie->isValid()){
        //qDebug() << "loading.gif didn't load";
        qApp->quit();
    }

    //Play Loading GIF set Status Label
    ui->processLabel->setMovie(movie);
    movie->start();
    ui->statusLabel->setText("Checking Version");

    checkVersion();
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::processReply(){
    QByteArray ba = netReply->readAll();
    QString result(ba);
    result = result.split("::")[0];
    qDebug() << result;

    QSettings settings;
    QString curVersion = settings.value("version", "").toString();

    //Check if diff in version
    if (curVersion.toDouble() < result.toDouble()){
        qDebug() << "Update Needed";
        doUpdate = true;
    }

    if (doUpdate){
        ui->statusLabel->setText("Downloading Update");
        downloadUpdate();
    }
    else {
        ui->statusLabel->setText("App is already up to date");
    }
}

void MainWindow::fileDownloaded(){
    QByteArray ba = netReply->readAll();
    QString result(ba);

    result = result.split("!!!:::")[1];

    ba.clear();
    ba.append(result);

    QFile file("_tmp");

    result = QByteArray::fromBase64(ba);

    if(file.open(QIODevice::WriteOnly)){
        file.write(QByteArray::fromBase64(ba));
    }

    file.close();

    replaceBinary();
}

void MainWindow::launch(){
    QString exeFileName;
    exeFileName = "webzvpn.exe";
    ui->statusLabel->setText("Updated!");
    movie->stop();
//    QProcess::startDetached("webzvpn.exe");
#ifdef Q_OS_WIN
    int result = (int)::ShellExecuteA(0, "runas", exeFileName.toUtf8().constData(), 0, 0, SW_SHOWNORMAL);

    if (result <= 32){
        // TODO: Error Handling
    }
#else
    if (!QProcess::startDetached(exeFileName)){
        // TODO: Error Handling
    }
#endif
    qApp->quit();
}

void MainWindow::checkVersion(){
    netReply.reset(netManager.get(QNetworkRequest(QUrl("http://webzvpn.ru/update.php"))));
    connect(netReply.get(), SIGNAL(finished()), this, SLOT(processReply()));
}

void MainWindow::downloadUpdate(){
    netReply.reset(netManager.get(QNetworkRequest(QUrl("http://webzvpn.ru/update.php"))));
    connect(netReply.get(), SIGNAL(finished()), this, SLOT(fileDownloaded()));

    launchWebzvpn();
}

void MainWindow::replaceBinary(){
    //TODO: Error Handling
    QFile::rename("webzvpn.exe", "tmp.dat");
    QFile::rename("_tmp", "webzvpn.exe");
    QFile::remove("tmp.dat");
}

void MainWindow::launchWebzvpn(){
    QTimer::singleShot(2000, this, SLOT(launch()));
}
