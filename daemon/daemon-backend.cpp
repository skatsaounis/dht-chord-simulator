#include "daemon-backend.hpp"
#include <unistd.h>
#include <string>
#include <iostream>
#include <cstring>
#include <system_error>

using namespace std;

bool Daemon::is_running() const {
    return _m_is_running;
}

void Daemon::terminate() {
    _m_is_running = false;
}

void Daemon::init_node(const string& node_id, const string& replica_factor, const string& consistency) try {
    char command[31];
    strncpy(command, "/usr/local/share/dsemu/node.py", 30);

    char arg_id[node_id.length() + 1];
    strncpy(arg_id, node_id.c_str(), node_id.length());

    char arg_rep[replica_factor.length() + 1];
    strncpy(arg_rep, replica_factor.c_str(), replica_factor.length());

    char arg_cons[consistency.length() + 1];
    strncpy(arg_cons, consistency.c_str(), consistency.length());

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
    //TODO: Send terminate command to node
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
