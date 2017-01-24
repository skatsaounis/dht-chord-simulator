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
    void status();
    void start();
    void terminate();
    void list_nodes();
    void list_ring(unsigned node_id);
    void init_node(unsigned node_id, unsigned n_replicas, ConsistencyTypes consistency);
    void terminate_node(unsigned node_id);
    void join(unsigned node_id);
    void depart(unsigned node_id);
    void query(const std::string& key);
    void insert(const std::string& key, const std::string& value);
    void remove(const std::string& key);
};

#endif  // _DSEMU_DAEMON_FRONTEND_HPP_
