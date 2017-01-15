#ifndef _DSEMU_DAEMON_FRONTEND_HPP_
#define _DSEMU_DAEMON_FRONTEND_HPP_

#include "globals.hpp"
#include <string>

class DaemonInterface {
    int sfd = 0;
    bool _m_is_connected = false;

    void _send_message(const std::string& message) const;

public:
    DaemonInterface();
    ~DaemonInterface();

    bool isRunning();
    void start();
    void terminate();
    void init_node(unsigned node_id, unsigned n_replicas, ConsistencyTypes consistency);
    void terminate_node(unsigned node_id);
};

#endif  // _DSEMU_DAEMON_FRONTEND_HPP_
