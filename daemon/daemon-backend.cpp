#include "daemon-backend.hpp"
#include "globals.hpp"
#include "commands.hpp"
#include "utils.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sstream>
#include <string>
#include <random>
#include <iostream>
#include <cstring>
#include <system_error>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;


bool MessageQueue::iterator::is_end() const
{
    return _m_is_endit;
}

MessageQueue::iterator::reference
MessageQueue::iterator::message()
{
    if (!_m_has_message){
        _m_message = parent._get_message();
        _m_has_message = true;
    }
    return _m_message;
}

MessageQueue::iterator
MessageQueue::iterator::createEnd(MessageQueue& parent_queue,
                                  MessageQueue::iterator::container& msg_queue)
{
    auto obj = iterator(parent_queue, msg_queue);
    obj._m_is_endit = true;
    return obj;
}

MessageQueue::iterator::iterator(MessageQueue& parent_queue,
                                 MessageQueue::iterator::container& msg_queue):
    parent(parent_queue), mqueue(msg_queue)
{
    current = mqueue.begin();
}

MessageQueue::iterator::~iterator()
{
    mqueue.swap(recycled);
}

bool MessageQueue::iterator::operator==(const iterator& other)
{
    if (&parent != &other.parent) return false;
    if (is_end() && other.is_end())
        return true;
    else if (is_end() || other.is_end()) {
        const iterator& working = (is_end()? other : *this);
        return working.mqueue.empty() && working.parent.is_open();
    }
    else
        return false;
}

bool MessageQueue::iterator::operator!=(const iterator& other)
{
    return !this->operator==(other);
}

MessageQueue::iterator&
MessageQueue::iterator::operator++()
{
    if (mqueue.empty())
        _m_has_message = false;
    else
        mqueue.pop_front();
    return *this;
}

MessageQueue::iterator::reference
MessageQueue::iterator::operator*()
{
    if (mqueue.empty())
        return message();
    else
        return mqueue.front();
}

MessageQueue::iterator::pointer
MessageQueue::iterator::operator->()
{
    return &this->operator*();
}

void MessageQueue::iterator::recycle_message()
{
    recycled.emplace_back(this->operator*());
}


string MessageQueue::_get_message() const {
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
    return ss.str();
}

MessageQueue::~MessageQueue()
{
    close();
}

void MessageQueue::open(std::string sockpath)
{
    close();
    _m_socket_path = sockpath;

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) throw system_error(errno, system_category(), "Cannot create initial socket.");

    struct sockaddr_un saddr;
    memset(&saddr, 0, sizeof(struct sockaddr_un));
    saddr.sun_family = AF_UNIX;
    strncpy(saddr.sun_path, _m_socket_path.c_str(), sizeof(saddr.sun_path) - 1);

    int ret = bind(sfd, (struct sockaddr*) &saddr, sizeof(struct sockaddr_un));
    if (ret == -1) throw system_error(errno, system_category(), "Cannot bind initial socket to file.");

    ret = listen(sfd, 50);
    if (ret == -1) throw system_error(errno, system_category(), "Cannot listen from initial socket.");

    _m_is_open = true;
}

void MessageQueue::close()
{
    if (!_m_is_open) return;

    shutdown(sfd, SHUT_RDWR);
    unlink(_m_socket_path.c_str());

    _m_is_open = false;
}

bool MessageQueue::is_open() const
{
    return _m_is_open;
}

MessageQueue::iterator
MessageQueue::begin()
{
    return iterator(*this, message_queue);
}

MessageQueue::iterator
MessageQueue::end()
{
    return iterator::createEnd(*this, message_queue);
}


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

/// Block until a notification is sent about
/// the completion of the specified task.
void Daemon::_wait_for_notify(Commands command) const try {
    for (auto it = message_queue.begin(); it != message_queue.end(); ++it) {
        auto vars = json::parse(*it);
        if (vars.at("cmd") == "notify-daemon" &&
            to_command_enum(vars.at("action")) == command) {
            if (vars.at("action") != "depart")
                cout << "[node-" << vars.value("node", "") << "] Answer has been received from node " << vars.value("sender", "") << endl;
            if (vars.at("action") == "insert")
                cout << "[node-" << vars.value("node", "") << "] Key has been inserted" << endl;
            else if (vars.at("action") == "delete") {
                if (vars.at("args").at("value") != "nf")
                    cout << "[node-" << vars.value("node", "") << "] " << vars.at("args").value("key", "") << " has been deleted" << endl;
                else
                    cout << "[node-" << vars.value("node", "") << "] Key " << vars.at("args").value("key", "") << " not found" << endl;
            }
            else if (vars.at("action") == "query") {
                if (vars.at("args").at("value") != "nf")
                    cout << "[node-" << vars.value("node", "") << "][answer] Key " << vars.at("args").value("key", "") << " has value " << vars.at("args").value("value", "") << endl;
                else
                    cout << "[node-" << vars.value("node", "") << "][answer] Key " << vars.at("args").value("key", "") << " not found" << endl;
            }
            break;
        }
        else
            it.recycle_message();
    }
} catch (const exception&) {
    throw_with_nested(runtime_error("While waiting for command completion notification"));
}


/// Send message for graceful termination to a node.
void Daemon::_send_terminate(const string& node_id) const try {
    json jmsg = {
        {"cmd", "depart-cmd"},
        {"sender", "daemon"},
        {"args", ""}
    };
    _send_message(node_id, jmsg.dump());
    _wait_for_notify(Commands::Depart);
} catch (const exception& e) {
    cerr << "Error during the termination of node " << node_id << ':' << endl;
    print_exception(e);
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

Daemon::Daemon()
{
    int ret = daemon(0, 1);
    if (ret == -1) throw system_error(errno, system_category(), "Cannot daemonize process.");
    message_queue.open(socket_path);
}

/// Get the replica factor for this session.
unsigned Daemon::get_replica_factor() const {
    if (_m_replica_factor == 0) return 1;  // default value
    return _m_replica_factor;
}

/// Set the replica factor for this session.
void Daemon::set_replica_factor(unsigned r) {
    if (r == 0) return;  // 0 means, do not change
    if (get_replica_factor() == r) return;
    _m_replica_factor = r;
    cout << "[daemon] Replica factor set to " << get_replica_factor() << endl;
}

/// Terminate the daemon instance,
/// releasing all nodes.
void Daemon::terminate() {
    // Send termination message to all nodes.
    // Termination sequence will continue regardless of
    // whether the nodes succeed to receive the message.
    for (auto&& n: node_ids) try {
        _send_terminate(n.first);
    } catch(const exception& e) {
        cerr << "Error occured while trying to terminate node " << n.first << " (id: " << n.second << "):" << endl;
        print_exception(e);
    }
    message_queue.close();
    node_ids.clear();
}

/// Initialize a node.
void Daemon::init_node(const string& node_id, const string& n_replicas,
                       const string& consistency, unsigned verbosity) try {
    set_replica_factor(stoi(n_replicas));
    string replica_factor(to_string(get_replica_factor()));

    char command[31];
    strncpy(command, "/usr/local/share/dsemu/node.py", sizeof(command));

    char arg_id[node_id.length() + 1];
    strncpy(arg_id, node_id.c_str(), sizeof(arg_id));

    char arg_rep[replica_factor.length() + 1];
    strncpy(arg_rep, replica_factor.c_str(), sizeof(arg_rep));

    char arg_cons[consistency.length() + 1];
    strncpy(arg_cons, consistency.c_str(), sizeof(arg_cons));

    char arg_verbosity[2];
    strncpy(arg_verbosity, to_string(verbosity).c_str(), sizeof(arg_verbosity));

    char * newargv[] = {
        command,
        arg_id,
        arg_rep,
        arg_cons,
        arg_verbosity,
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

/// Request status report from a node.
void Daemon::list_ring(const string& node_id) try {
    json jmsg = {
        {"cmd", "list-cmd"},
        {"sender", "daemon"},
        {"args", ""}
    };
    _send_message(node_id, jmsg.dump());
} catch(const exception&) {
    throw_with_nested(runtime_error("While requesting ring node listing"));
}

/// Request status report from all nodes.
void Daemon::list_all_nodes() {
    for (auto&& n: node_ids)
        list_ring(n.first);
}

/// Request the ring to allow a new node to join.
void Daemon::join(const string& node_id) try {
    json jmsg = {
        {"cmd", "join-cmd"},
        {"sender", "daemon"},
        {"args", {
            {"socket_fd", _pick_random_node(node_id)}
        }}
    };
    _send_message(node_id, jmsg.dump());
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
            {"replica_counter", get_replica_factor()}
        }}
    };
    _send_message(_pick_random_node(), jmsg.dump());
    _wait_for_notify(Commands::Query);
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
    _wait_for_notify(Commands::Insert);
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
            {"replica_counter", get_replica_factor()}
        }}
    };
    _send_message(_pick_random_node(), jmsg.dump());
    _wait_for_notify(Commands::Delete);
} catch(const exception&) {
    throw_with_nested(runtime_error("While deleting key " + key));
}
