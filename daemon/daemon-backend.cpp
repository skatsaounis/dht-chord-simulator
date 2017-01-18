#include "daemon-backend.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>
#include <iostream>
#include <cstring>
#include <system_error>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;

void Daemon::_send_message(const string& node_id, const string& msg) const {
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) throw system_error(errno, system_category(), "Cannot create socket");

    struct sockaddr_un saddr;
    memset(&saddr, 0, sizeof(struct sockaddr_un));
    saddr.sun_family = AF_UNIX;
    strncpy(saddr.sun_path, ("/var/run/dsemu/" + node_id).c_str(), sizeof(saddr.sun_path) - 1);

    int ret = connect(sfd, (struct sockaddr*) &saddr, sizeof(saddr));
    if (ret < 0) throw system_error(errno, system_category(), "Cannot connect to node socket");

    int n = write(sfd, msg.c_str(), msg.length());
    if (n < 0) throw system_error(errno, system_category(), "Cannot write message to node socket");

     shutdown(sfd, SHUT_RDWR);
}

bool Daemon::is_running() const {
    return _m_is_running;
}

void Daemon::terminate() {
    _m_is_running = false;
}

void Daemon::init_node(const string& node_id, const string& replica_factor, const string& consistency) try {
    char command[31];
    strncpy(command, "/usr/local/share/dsemu/node.py", sizeof(command));

    char arg_id[node_id.length() + 1];
    strncpy(arg_id, node_id.c_str(), sizeof(arg_id));

    char arg_rep[replica_factor.length() + 1];
    strncpy(arg_rep, replica_factor.c_str(), sizeof(arg_rep));

    char arg_cons[consistency.length() + 1];
    strncpy(arg_cons, consistency.c_str(), sizeof(arg_cons));

    char * newargv[] = {
        command,
        arg_id,
        arg_rep,
        arg_cons,
        NULL
    };

    char * newenviron[] = {
        NULL
    };

    pid_t pid = fork();

    if (pid == 0) {
        execve(command, newargv, newenviron);
    }
    else if (pid > 0) {
        node_ids[node_id] = stoi(node_id);
        cout << "[daemon] Initialized node " << node_id << endl;
    }
    else
        throw system_error(errno, system_category(), "Failed to fork node process");
} catch(const exception&) {
    throw_with_nested(runtime_error("While initializing node " + node_id));
}

void Daemon::terminate_node(const string& node_id) try {
    node_ids.erase(node_id);
    json jmsg = {
        {"cmd", "depart-cmd"},
        {"sender", "daemon-socket-id"},
        {"args", ""}
    };
    _send_message(node_id, jmsg.dump());
} catch(const exception&) {
    throw_with_nested(runtime_error("While terminating node " + node_id));
}

void Daemon::list_nodes() try {
    cout << "Node\tID" << endl;
    for (auto n: node_ids)
        cout << n.first << '\t' << n.second << endl;
} catch(const exception&) {
    throw_with_nested(runtime_error("While printing node list"));
}
