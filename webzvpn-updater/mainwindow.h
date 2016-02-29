#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include <memory>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QTimer>
#include <QMovie>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void processReply();
    void fileDownloaded();
    void launch();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager netManager;
    std::auto_ptr<QNetworkReply> netReply;

    QMovie *movie;

    void checkVersion();
    void downloadUpdate();
    void replaceBinary();
    void launchWebzvpn();

    bool doUpdate;
};

#endif // MAINWINDOW_H
