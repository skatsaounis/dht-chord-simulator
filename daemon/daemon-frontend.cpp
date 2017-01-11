#include "daemon-frontend.hpp"
#include "globals.hpp"
#include "daemon-main.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cerrno>
#include <cstring>
#include <system_error>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;


DaemonInterface::DaemonInterface() try {
    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) throw system_error(errno, system_category(), "Cannot create socket");

    struct sockaddr_un saddr;
    memset(&saddr, 0, sizeof(struct sockaddr_un));
    saddr.sun_family = AF_UNIX;
    strncpy(saddr.sun_path, socket_path.c_str(), sizeof(saddr.sun_path) - 1);

    int ret = connect(sfd, (struct sockaddr*) &saddr, sizeof(saddr));
    if (ret < 0)
        switch (errno) {
            case EISCONN:
                _m_is_connected = true;
                break;
            case ENOENT:  // daemon socket not created
            case EADDRNOTAVAIL:  // daemon socket not responding
            case ENETUNREACH:
            case ETIMEDOUT:
                _m_is_connected = false;
                break;
            default:
                throw system_error(errno, system_category(), "Cannot connect to daemon socket");
                break;
        }
    else
        _m_is_connected = true;
} catch(const exception&) {
    throw_with_nested(runtime_error("While connecting with daemon"));
}

DaemonInterface::~DaemonInterface() try {
     shutdown(sfd, SHUT_RDWR);
} catch(const exception&) {
    throw_with_nested(runtime_error("While disconnecting from daemon"));
}

bool DaemonInterface::isRunning() {
    return _m_is_connected;
}

void DaemonInterface::start() {
    daemon_main();
}

void DaemonInterface::terminate() try {
    json jmsg = {
       {"cmd", "terminate"}};
    string msg = jmsg.dump();
    int n = write(sfd, msg.c_str(), msg.length());
    if (n < 0) throw system_error(errno, system_category(), "Cannot write message to daemon socket");
} catch(const exception&) {
    throw_with_nested(runtime_error("While sending termination message to daemon"));
}
