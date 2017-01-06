#include "daemon-main.hpp"
#include "globals.hpp"
#include "utils.hpp"
#include "daemon-backend.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstdlib>
#include <system_error>
#include <cerrno>
#include <memory>
#include <cstring>
#include <iostream>

using namespace std;

void daemon_main() try {

    int ret = daemon(0, 1);
    if (ret == -1) throw system_error(errno, system_category(), "Cannot daemonize process.");

    //redirect log?

    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) throw system_error(errno, system_category(), "Cannot create initial socket.");

    struct sockaddr_un saddr;
    memset(&saddr, 0, sizeof(struct sockaddr_un));
    saddr.sun_family = AF_UNIX;
    strncpy(saddr.sun_path, socket_path.c_str(), sizeof(saddr.sun_path) - 1);

    // ret = bind(sfd, (struct sockaddr*) &saddr, sizeof(struct sockaddr_un));
    // if (ret == -1) throw system_error(errno, system_category(), "Cannot bind initial socket to file.");

    // ret = listen(sfd, 50);
    // if (ret == -1) throw system_error(errno, system_category(), "Cannot listen from initial socket.");

    //main loop
    Daemon daemon;
    daemon.init_node(4, 42);

    shutdown(sfd, SHUT_RDWR);
    unlink(socket_path.c_str());
}
catch (const exception& e) {
    cerr << "The daemon crashed:" << endl;
    print_exception(e);
    exit(EXIT_FAILURE);
}
