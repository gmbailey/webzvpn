#include "common.h"

#include <QUrl>
#include <QDesktopServices>
#include <QSettings>
#include <QApplication>
#include <QSslConfiguration>
#include <QSslSocket>

bool IsValidIp(const QString & ip)
{
    bool valid = false;
    int points = ip.count('.');
    if (3 == points)
    {
        // do parse
        QStringList ar = ip.split('.', QString::SkipEmptyParts);
        if (ar.length() == 4)
        {
            bool ok = true;
            for (int k = 0; k < 4 && ok; ++k)
            {
                int v = ar[k].toInt(&ok);
                if (!(0 <= v && v <= 255))
                {
                    ok = false;
                    break;
                }
            }
            if (ok)
                valid = ok;
        }
    }
    return valid;
}


bool OpenUrl(const char * url)
{
    bool sucses = QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
    if (!sucses)
        ;
    return sucses;
}

bool OpenUrl_Support()
{
    return OpenUrl("https://webzvpn.ru");
}

bool OpenUrl_Panel()
{
    return OpenUrl("https://webzvpn.ru");
}


bool OpenUrl_Bug()
{
    return OpenUrl("https://webzvpn.ru");
}

bool OpenUrl_Update()
{
    return OpenUrl("https://webzvpn.ru");
}

// maybe empty str >>> false
// true only for numberic: [1-65535]
bool IsValidPort(const QString & s)
{
    bool ok;
    int v = s.toInt(&ok);
    if (ok)
    if (v < 1 || v > 65535)
        ok = false;
    return ok;
}

static QString m_sSettingsFile;
const QString & GetSettingsFn()
{
    if (m_sSettingsFile.isEmpty())
        m_sSettingsFile = //QApplication::applicationDirPath().left(1) +
                ":/webzvpn-settings.ini";
    return m_sSettingsFile;
}

void SaveCb(const char * name, bool val)
{
    SETTINGS_OBJ;
    settings.setValue(name, val);
}

QString EscapePsw(const QString & raw)
{
    QString s;
    for (int k = 0; k < raw.length(); ++k)
    {
        if (raw.at(k) == '\\' )
        {
            s += "\\\\";
        }
        else
        {
            if (raw.at(k) == '"')
            {
                s += "\\\"";
            }
            else
            {
                s += raw.at(k);
            }
        }
    }
    return s;
}

QNetworkRequest BuildRequest(const QUrl & u)
{
    QNetworkRequest req(u);
    QSslConfiguration conf = req.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    req.setSslConfiguration(conf);
    return req;
}

