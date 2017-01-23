#include "daemon-main.hpp"
#include "globals.hpp"
#include "utils.hpp"
#include "daemon-backend.hpp"
#include "commands.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstdlib>
#include <system_error>
#include <cerrno>
#include <memory>
#include <cstring>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;

void daemon_main() try {

    int ret = daemon(0, 1);
    if (ret == -1) throw system_error(errno, system_category(), "Cannot daemonize process.");

    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) throw system_error(errno, system_category(), "Cannot create initial socket.");

    struct sockaddr_un saddr;
    memset(&saddr, 0, sizeof(struct sockaddr_un));
    saddr.sun_family = AF_UNIX;
    strncpy(saddr.sun_path, socket_path.c_str(), sizeof(saddr.sun_path) - 1);

    ret = bind(sfd, (struct sockaddr*) &saddr, sizeof(struct sockaddr_un));
    if (ret == -1) throw system_error(errno, system_category(), "Cannot bind initial socket to file.");

    ret = listen(sfd, 50);
    if (ret == -1) throw system_error(errno, system_category(), "Cannot listen from initial socket.");

    // Main service loop.

    Daemon daemon;
    cout << "[daemon] Starting" << endl;
    while (daemon.is_running()) try {
        // Accept incoming connection
        struct sockaddr_un cliaddr;
        socklen_t cliaddr_size = sizeof(cliaddr);
        int csfd = accept(sfd, (struct sockaddr*) &cliaddr, &cliaddr_size);
        if (csfd == -1) throw system_error(errno, system_category(), "Could not accept incoming connection");
        // Receive request
        stringstream ss;
        while (true) {
            char buffer[256];
            int n_read = read(csfd, buffer, 255);
            if (n_read < 0) throw system_error(errno, system_category(), "Read error from client socket");
            if (n_read == 0) break;
            buffer[n_read] = '\0';
            string chunk = buffer;
            ss << chunk;
        }
        // Parse request
        auto vars = json::parse(ss.str());
        // Process request
        Commands cmd = to_command_enum(vars.at("cmd"));
        switch(cmd) {
            case Commands::Status:
                cout << "[daemon] Status: Running." << endl;
                break;
            case Commands::Start:
                // Initialize node
                daemon.init_node(vars.at("node"), vars.at("replicas"), vars.at("consistency"));
                break;
            case Commands::Terminate: {
                auto nodeit = vars.find("node");
                if (nodeit == vars.end()) {
                    // Terminate daemon
                    cout << "[daemon] Terminating" << endl;
                    daemon.terminate();
                } else {
                    // Terminate node
                    daemon.terminate_node(*nodeit);
                }
            }   break;
            case Commands::List:
                daemon.list_nodes();
                break;
            case Commands::Join:
                daemon.join(vars.at("node"));
                break;
            case Commands::Depart:
                daemon.depart(vars.at("node"));
                break;
            case Commands::Query:
                daemon.query(vars.at("key"));
                break;
            case Commands::Insert:
                daemon.insert(vars.at("key"), vars.at("value"));
                break;
            case Commands::Delete:
                daemon.remove(vars.at("key"));
                break;
            default:
                break;
        }
    } catch (const exception& e) {
        cerr << "Could not process client request:" << endl;
        print_exception(e);
    }

    shutdown(sfd, SHUT_RDWR);
    unlink(socket_path.c_str());
}
catch (const exception& e) {
    cerr << "The daemon crashed:" << endl;
    print_exception(e);
    exit(EXIT_FAILURE);
}
