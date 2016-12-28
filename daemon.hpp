#ifndef _DSEMU_DAEMON_HPP_
#define _DSEMU_DAEMON_HPP_

class Daemon {
    void* _m_connection = nullptr;

    void _connect();
    void _disconnect();

public:
    Daemon();
    ~Daemon();
};

#endif  // _DSEMU_DAEMON_HPP_
