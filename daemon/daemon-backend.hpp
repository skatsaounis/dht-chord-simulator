#ifndef _DSEMU_DAEMON_BACKEND_HPP_
#define _DSEMU_DAEMON_BACKEND_HPP_

#include <vector>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>

class Daemon {
    bool _m_is_running = true;
    std::unordered_map<std::string, unsigned> node_ids;
    mutable std::vector<std::thread> threads;
    mutable std::timed_mutex node_termination_mutex;
    mutable std::string _node_being_terminated;

    void _send_message(const std::string& node_id, const std::string& msg) const;
    void _send_terminate(const std::string& node_id) const;
    void _send_terminate_proc(const std::string& node_id) const;
    void _force_node_cleanup(const std::string& node_id) const;
    void _force_node_cleanup(const std::string& node_id, const std::string& reason) const;
    std::string _pick_random_node();
    std::string _pick_random_node(const std::string& excluded_node_id);

public:

    bool is_running() const;

    void terminate();
    void init_node(const std::string& node_id, const std::string& replica_factor, const std::string& consistency);
    void terminate_node(const std::string& node_id);
    void list_nodes();
    void list_ring(const std::string& node_id);
    void join(const std::string& node_id);
    void depart(const std::string& node_id);
    void query(const std::string& key);
    void insert(const std::string& key, const std::string& value);
    void remove(const std::string& key);
    void notify(const std::string& action);
};

#endif  // _DSEMU_DAEMON_BACKEND_HPP_
