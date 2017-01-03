#include "daemon-frontend.hpp"
#include <stdexcept>

using namespace std;

namespace {

class Connection {
    /* data */

public:
    Connection() = default;
    ~Connection() = default;
};

}  /* Unnamed namespace */


void Daemon::_connect() try {
    _m_connection = new Connection();
} catch(const exception&) {
    throw_with_nested(runtime_error("While connecting with daemon"));
}

void Daemon::_disconnect() try {
    if (_m_connection)
        delete static_cast<Connection*>(_m_connection);
    _m_connection = nullptr;
} catch(const exception&) {
    throw_with_nested(runtime_error("While disconnecting from daemon"));
}

Daemon::Daemon() try {
    _connect();
} catch(const exception&) {
    throw_with_nested(runtime_error("While constructing Daemon abstraction"));
}

Daemon::~Daemon() try {
    _disconnect();
} catch(const exception&) {
    throw_with_nested(runtime_error("While destructing Daemon abstraction"));
}
