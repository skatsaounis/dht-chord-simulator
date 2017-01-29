#include "daemon-backend.hpp"
#include "globals.hpp"
#include "utils.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>
#include <random>
#include <iostream>
#include <cstring>
#include <system_error>
#include <thread>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;

/// Send a message to a node.
void Daemon::_send_message(const string& node_id, const string& msg) const {
    const string node_sock_path = socket_dir + node_id;

    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) throw system_error(errno, system_category(), "Cannot create socket");

    struct sockaddr_un saddr;
    memset(&saddr, 0, sizeof(struct sockaddr_un));
    saddr.sun_family = AF_UNIX;
    strncpy(saddr.sun_path, node_sock_path.c_str(), sizeof(saddr.sun_path) - 1);

    int ret = connect(sfd, (struct sockaddr*) &saddr, sizeof(saddr));
    if (ret < 0) throw system_error(errno, system_category(), "Cannot connect to node socket " + node_sock_path);

    int n = write(sfd, msg.c_str(), msg.length());
    if (n < 0) throw system_error(errno, system_category(), "Cannot write message to node socket " + node_sock_path);

     shutdown(sfd, SHUT_RDWR);
}

/// Send message for graceful termination to a node.
void Daemon::_send_terminate(const string& node_id) const {
    threads.emplace_back(&Daemon::_send_terminate_proc, this, node_id);
}

/// Detachable concurrent process for sending termination message
/// to a node when the termination action becomes available.
void Daemon::_send_terminate_proc(const string& node_id) const try {
    json jmsg = {
        {"cmd", "depart-cmd"},
        {"sender", "daemon-socket-id"},
        {"args", ""}
    };
    bool prev_term_succ = node_termination_mutex.try_lock_for(chrono::milliseconds(1000));
    if (!prev_term_succ) _force_node_cleanup(_node_being_terminated, "Node unresponsive during termination.");
    _node_being_terminated = node_id;
    _send_message(node_id, jmsg.dump());
} catch (const exception& e) {
    cerr << "Error during the termination of node " << node_id << ':' << endl;
    print_exception(e);
}

/// Immediately halt a node and force the cleanup of its elements.
void Daemon::_force_node_cleanup(const string& node_id) const {
    //TODO kill node process
    unlink((socket_dir + node_id).c_str());
}

/// Immediately halt a node and force the cleanup of its elements.
/// Explain the reason of termination with an error message.
void Daemon::_force_node_cleanup(const string& node_id, const string& reason) const {
    cerr << "Killing node " << node_id << ": " << reason << endl;
    _force_node_cleanup(node_id);
}

/// Select a random active node.
string Daemon::_pick_random_node() try {
    // Algorithm from http://stackoverflow.com/a/31522686
    if (node_ids.empty()) throw runtime_error("There are no active nodes");
    // Generate a random node name.
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<> dist(32, 126);
    string key(16, ' ');
    for (auto& c: key) c = static_cast<char>(dist(gen));
    // If node exists, return it, else insert it.
    auto ins = node_ids.insert(make_pair(key, 0));
    if (!ins.second) return ins.first->first;
    // Get iterator to next element (or the first, if it was the last).
    auto it = ins.first;
    it++;
    if (it == node_ids.end()) it = node_ids.begin();
    auto res = it->first;
    // Remove the inserted random node name.
    node_ids.erase(ins.first);
    // Return the element that was next to the random name.
    return res;
} catch(const exception&) {
    throw_with_nested(runtime_error("While selecting a random node"));
}

/// Select a random active node, except the one specified.
string Daemon::_pick_random_node(const string& node_id) try {
    if (node_ids.empty()) throw runtime_error("There are no active nodes");
    node_ids.at(node_id);
    node_ids.erase(node_id);
    auto res = _pick_random_node();
    node_ids[node_id] = stoi(node_id);
    return res;
} catch(const exception&) {
    throw_with_nested(runtime_error("While removing node from random selection"));
}

/// Check whether the daemon is running,
/// and able to process requests.
bool Daemon::is_running() const {
    return _m_is_running;
}

/// Terminate the daemon instance,
/// releasing all nodes.
void Daemon::terminate() {
    _m_is_running = false;
    // Send termination message to all nodes.
    // Termination sequence will continue regardless of
    // whether the nodes succeed to receive the message.
    for (auto&& n: node_ids) try {
        _send_terminate(n.first);
    } catch(const exception& e) {
        cerr << "Error occured while trying to terminate node " << n.first << " (id: " << n.second << "):" << endl;
        print_exception(e);
    }
    node_ids.clear();
    // Finish any remaining jobs.
    for (auto& t: threads)
        t.join();
}

/// Initialize a node.
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

/// Terminate a node gracefully.
void Daemon::terminate_node(const string& node_id) try {
    node_ids.erase(node_id);
    _send_terminate(node_id);
} catch(const exception&) {
    throw_with_nested(runtime_error("While terminating node " + node_id));
}

/// Output a list of all active nodes.
void Daemon::list_nodes() try {
    cout << "Node\tID" << endl;
    for (auto&& n: node_ids)
        cout << n.first << '\t' << n.second << endl;
} catch(const exception&) {
    throw_with_nested(runtime_error("While printing node list"));
}

/// Request node ring to list itself in order.
void Daemon::list_ring(const string& node_id) try {
    json jmsg = {
        {"cmd", "list-cmd"},
        {"sender", "daemon-socket-id"},
        {"args", ""}
    };
    _send_message(node_id, jmsg.dump());
} catch(const exception&) {
    throw_with_nested(runtime_error("While requesting ring node listing"));
}

/// Request the ring to allow a new node to join.
void Daemon::join(const string& node_id) try {
    json jmsg = {
        {"cmd", "join-cmd"},
        {"sender", "daemon-socket-id"},
        {"args", {
            {"socket_fd", node_id}
        }}
    };
    _send_message(_pick_random_node(node_id), jmsg.dump());
} catch(const exception&) {
    throw_with_nested(runtime_error("On join of node " + node_id));
}

/// Make a node depart from the ring.
void Daemon::depart(const string& node_id) {
    terminate_node(node_id);
}

/// Query the DHT for the value associated with a key.
void Daemon::query(const string& key) try {
    json jmsg = {
        {"cmd", "query-cmd"},
        {"sender", "daemon"},
        {"args", {
            {"initial_sender", "daemon"},
            {"key", key},
            {"replica_counter", 2}  //FIXME 1.0
        }}
    };
    _send_message(_pick_random_node(), jmsg.dump());
} catch(const exception&) {
    throw_with_nested(runtime_error("While querying key " + key));
}

// Insert a key - value pair into the DHT.
void Daemon::insert(const string& key, const string& value) try {
    json jmsg = {
        {"cmd", "insert-cmd"},
        {"sender", "daemon"},
        {"args", {
            {"key", key},
            {"value", value},
            {"initial_sender", "daemon"},
        }}
    };
    _send_message(_pick_random_node(), jmsg.dump());
} catch(const exception&) {
    throw_with_nested(runtime_error("While inserting key " + key + " with value " + value));
}

/// Delete a key from the DHT.
void Daemon::remove(const string& key) try {
    json jmsg = {
        {"cmd", "delete-cmd"},
        {"sender", "daemon"},
        {"args", {
            {"initial_sender", "daemon"},
            {"key", key},
            {"replica_counter", 2}  //FIXME 1.0
        }}
    };
    _send_message(_pick_random_node(), jmsg.dump());
} catch(const exception&) {
    throw_with_nested(runtime_error("While deleting key " + key));
}

void Daemon::notify(const string& action) {
    if (action == "depart") node_termination_mutex.unlock();
}
