#include "options.hpp"
#include "daemon-frontend.hpp"
#include "utils.hpp"
#include <stdexcept>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv) try {
    Options options;
    options.parse(argc, argv);
    DaemonInterface daemon;
    switch (options.command()) {
        case Commands::Status:
            if (daemon.isRunning())
                cout << "The daemon is running." << endl;
            else
                cout << "The daemon is not running." << endl;
            break;
        case Commands::Start:
            if (!daemon.isRunning())
                daemon.start();
            break;
        case Commands::Terminate:
            if (daemon.isRunning())
                daemon.terminate();
            break;
        case Commands::Help:
        case Commands::Version:
            // Handled by the options parser.
            break;
        default:
            cerr << "Function not implemented" << endl;
    }
}
catch (const exception& e) {
    cerr << "The interface program crashed:" << endl;
    print_exception(e);
    exit(EXIT_FAILURE);
}
