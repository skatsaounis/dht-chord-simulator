#ifndef _DSEMU_DAEMON_BACKEND_HPP_
#define _DSEMU_DAEMON_BACKEND_HPP_

#include <string>

struct Daemon {
    void init_node(unsigned node_id, unsigned replica_factor, const std::string& consistency);
};

#endif  // _DSEMU_DAEMON_BACKEND_HPP_
