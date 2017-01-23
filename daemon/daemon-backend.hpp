#ifndef _DSEMU_DAEMON_BACKEND_HPP_
#define _DSEMU_DAEMON_BACKEND_HPP_

#include <string>
#include <unordered_map>

class Daemon {
    bool _m_is_running = true;
    bool _m_is_listing_ring = false;
    std::string _m_ring_lister;
    std::unordered_map<std::string, unsigned> node_ids;

    void _send_message(const std::string& node_id, const std::string& msg) const;
    void _send_terminate(const std::string& node_id) const;
    std::string _pick_random_node();

public:

    bool is_running() const;
    bool is_ring_listing_in_progress() const;

    void terminate();
    void init_node(const std::string& node_id, const std::string& replica_factor, const std::string& consistency);
    void terminate_node(const std::string& node_id);
    void list_nodes();
    void list_ring();
    void list_ring_stop();
    void join(const std::string& node_id);
    void depart(const std::string& node_id);
    void query(const std::string& key);
    void insert(const std::string& key, const std::string& value);
    void remove(const std::string& key);
};

#endif  // _DSEMU_DAEMON_BACKEND_HPP_
