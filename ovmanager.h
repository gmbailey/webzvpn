#ifndef OVMANAGER_H
#define OVMANAGER_H

#include <memory>

#include "common.h"
#include "thread_oldip.h"
#include <QString>

#define PINGWORKERS_NUM 16
#define PINGWORKER_MAX_TIMEOUT 2000

#ifndef uint64_t
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#ifdef WIN32
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include <time.h>
#include <errno.h>

#endif		// #ifndef uint64_t

class OvManager
{
public:
    static OvManager * Instance();
    static bool isExists() { return (_inst.get() != NULL); }
    static void cleanup();
    ~OvManager();

    uint64_t getRnd64();
//    void processOldIp(QString ip);
//    void determineOldIp();

private:
    OvManager();
    static std::auto_ptr<OvManager> _inst;
    bool seeded;
    void doSeed();
//    QString newip;
//    QString oldip;

};

#endif // OVMANAGER_H
