#include "daemon-backend.hpp"
#include <unistd.h>
#include <string>
#include <iostream>
#include <cstring>
#include <system_error>

using namespace std;

void Daemon::init_node(unsigned node_id, unsigned replica_factor, const string& consistency) try {
    char command[31];
    strncpy(command, "/usr/local/share/dsemu/node.py", 30);

    char arg_id[6];
    strncpy(arg_id, to_string(node_id).c_str(), 5);
    
    char arg_rep[6];
    strncpy(arg_rep, to_string(replica_factor).c_str(), 5);

    char arg_cons[consistency.size() + 1];
    strncpy(arg_cons, consistency.c_str(), consistency.size());

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
        cout << "[daemon] Initialized node " << node_id;
    }
    else
        throw system_error(errno, system_category(), "Failed to fork node process");
} catch(const exception&) {
    throw_with_nested(runtime_error("While initializing node " + to_string(node_id)));
}
