#ifndef _DSEMU_DAEMON_BACKEND_HPP_
#define _DSEMU_DAEMON_BACKEND_HPP_

#include <string>
#include <unordered_map>

class Daemon {
    bool _m_is_running = true;
    std::unordered_map<std::string, unsigned> node_ids;

    void _send_message(const std::string& node_id, const std::string& msg) const;

public:

    bool is_running() const;

    void terminate();
    void init_node(const std::string& node_id, const std::string& replica_factor, const std::string& consistency);
    void terminate_node(const std::string& node_id);
    void list_nodes();
};

#endif  // _DSEMU_DAEMON_BACKEND_HPP_
