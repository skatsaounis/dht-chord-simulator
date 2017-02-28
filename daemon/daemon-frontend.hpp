#ifndef _DSEMU_DAEMON_FRONTEND_HPP_
#define _DSEMU_DAEMON_FRONTEND_HPP_

class DaemonInterface {
    int sfd = 0;
    bool _m_is_connected = false;

public:
    DaemonInterface();
    ~DaemonInterface();

    bool isRunning();
    void start();
    void terminate();
};

#endif  // _DSEMU_DAEMON_FRONTEND_HPP_
