#ifndef _DSEMU_DAEMON_BACKEND_HPP_
#define _DSEMU_DAEMON_BACKEND_HPP_

#include "commands.hpp"
#include <vector>
#include <string>
#include <unordered_map>

class Daemon {
    int sfd = 0;
    bool _m_is_running = true;
    unsigned _m_replica_factor = 0;
    std::unordered_map<std::string, unsigned> node_ids;

    void _send_message(const std::string& node_id, const std::string& msg) const;
    void _send_terminate(const std::string& node_id) const;
    void _wait_for_notify(Commands) const;
    std::string _pick_random_node();
    std::string _pick_random_node(const std::string& excluded_node_id);

public:

    Daemon();
    ~Daemon();

    bool is_running() const;
    std::string get_message() const;
    unsigned get_replica_factor() const;
    void set_replica_factor(unsigned);

    void terminate();
    void init_node(const std::string& node_id, const std::string& replica_factor,
        const std::string& consistency, unsigned verbosity);
    void terminate_node(const std::string& node_id);
    void list_nodes();
    void list_ring(const std::string& node_id);
    void list_all_nodes();
    void join(const std::string& node_id);
    void depart(const std::string& node_id);
    void query(const std::string& key);
    void insert(const std::string& key, const std::string& value);
    void remove(const std::string& key);
};

#endif  // _DSEMU_DAEMON_BACKEND_HPP_
