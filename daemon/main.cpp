#include "options.hpp"
#include "daemon-frontend.hpp"
#include "utils.hpp"
#include <stdexcept>
#include <iostream>
#include <string>

using namespace std;

struct CommandError: runtime_error {
    using runtime_error::runtime_error;
};

int main(int argc, char** argv) try {
    Options options;
    options.parse(argc, argv);
    DaemonInterface daemon;

    auto check_daemon_running = [&daemon]() {
        if (!daemon.isRunning())
            throw CommandError("Daemon is not running");
    };

    switch (options.command()) {
        case Commands::Status:
            if (daemon.isRunning())
                daemon.status();
            else
                cout << "The daemon is not running." << endl;
            break;
        case Commands::Start:
            if (options.was_node_specified()) {
                // Initialize node
                check_daemon_running();
                //TODO: check for already active node
                daemon.init_node(options.node(), options.n_replicas(), options.consistency());
            } else {
                // Start daemon
                if (!daemon.isRunning())
                    daemon.start(options.verbose());
            } break;
        case Commands::Terminate:
            if (options.was_node_specified()) {
                // Terminate node
                check_daemon_running();
                //TODO: check for already inactive node
                daemon.terminate_node(options.node());
            } else {
                // Terminate daemon
                if (daemon.isRunning())
                    daemon.terminate();
            } break;
        case Commands::List:
            check_daemon_running();
            if (options.list_mode() == "simple")
                daemon.list_nodes();
            else if (options.list_mode() == "ring")
                daemon.list_ring(options.node());
            else if (options.list_mode() == "*")
                daemon.list_all_nodes();
            break;
        case Commands::Join:
            check_daemon_running();
            daemon.join(options.node());
            break;
        case Commands::Depart:
            check_daemon_running();
            daemon.depart(options.node());
            break;
        case Commands::Query:
            check_daemon_running();
            daemon.query(options.key());
            break;
        case Commands::Insert:
            check_daemon_running();
            daemon.insert(options.key(), options.value());
            break;
        case Commands::Delete:
            check_daemon_running();
            daemon.remove(options.key());
            break;
        case Commands::Help:
        case Commands::Version:
            // Handled by the options parser.
            break;
        default:
            throw CommandError("Function not implemented");
    }
} catch (const CommandError& e) {
    cerr << e.what() << endl;
    exit(EXIT_FAILURE);
} catch (const exception& e) {
    cerr << "The interface program crashed:" << endl;
    print_exception(e);
    exit(EXIT_FAILURE);
}
