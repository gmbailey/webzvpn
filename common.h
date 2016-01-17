#ifndef COMMON_H
#define COMMON_H

#include <QDialog>
#include <QString>
#include <QSettings>
#include <QApplication>
#include <QNetworkRequest>

#include "stringize.hpp"

#include <map>
#include <set>
#include <string>
#include <vector>

#define PORT_FORWARD_MAX 5

extern QApplication * g_pTheApp;

bool IsValidIp(const QString & ip);
bool IsValidPort(const QString & s);

bool OpenUrl(const char * url);
bool OpenUrl_Support();
bool OpenUrl_Panel();
bool OpenUrl_Earn();
bool OpenUrl_Bug();
bool OpenUrl_Update();

//extern const QString & GetSettingsFn();

#define SETTINGS_OBJ QSettings settings("webzvpn", "Webz VPN Client")
//#define CB_SAVE_VALUE(name) do { SETTINGS_OBJ; settings.setValue(BOOST_PP_STRINGIZE(name), ui-> ##name## ->isChecked()); } while (0)

void SaveCb(const char * name, bool val);

typedef std::map<std::string, int> SIMap;
typedef std::map<int, int> IIMap;

typedef std::vector<unsigned int> UVec;
typedef std::set<unsigned int> USet;

// escape \ and "
QString EscapePsw(const QString & raw);

QNetworkRequest BuildRequest(const QUrl & u);

#endif // COMMON_H
