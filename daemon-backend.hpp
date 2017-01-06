#ifndef _DSEMU_DAEMON_BACKEND_HPP_
#define _DSEMU_DAEMON_BACKEND_HPP_

struct Daemon {
    void init_node(unsigned node_id, unsigned replica_factor);
};

#endif  // _DSEMU_DAEMON_BACKEND_HPP_
