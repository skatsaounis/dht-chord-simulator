#ifndef _DSEMU_DAEMON_BACKEND_HPP_
#define _DSEMU_DAEMON_BACKEND_HPP_

#include <string>

class Daemon {
    bool _m_is_running = true;

public:

    bool is_running() const;

    void terminate();
    void init_node(const std::string& node_id, const std::string& replica_factor, const std::string& consistency);
    void terminate_node(const std::string& node_id);
};

#endif  // _DSEMU_DAEMON_BACKEND_HPP_
