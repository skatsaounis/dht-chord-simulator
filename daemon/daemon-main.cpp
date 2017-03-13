#include "daemon-main.hpp"
#include "globals.hpp"
#include "utils.hpp"
#include "daemon-backend.hpp"
#include "commands.hpp"
#include <cstdlib>
#include <system_error>
#include <cerrno>
#include <memory>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;

void daemon_main(bool verbose) try {
    // Main service loop.
    Daemon daemon;
    cout << "[daemon] Starting" << endl;
    for (auto it = daemon.message_queue.begin(); it != daemon.message_queue.end(); ++it) try {
        auto& msg = *it;
        // Parse request
        auto vars = json::parse(msg);
        // Process request
        Commands cmd = to_command_enum(vars.at("cmd"));
        switch(cmd) {
            case Commands::Status:
                cout << "[daemon] Status: Running." << endl;
                break;
            case Commands::Start:
                // Initialize node
                daemon.init_node(vars.at("node"), vars.at("replicas"),
                    vars.at("consistency"), (verbose? 1:0));
                break;
            case Commands::Terminate: {
                auto nodeit = vars.find("node");
                if (nodeit == vars.end()) {
                    // Terminate daemon
                    cout << "[daemon] Terminating" << endl;
                    daemon.terminate();
                } else {
                    // Terminate node
                    daemon.terminate_node(*nodeit);
                }
            }   break;
            case Commands::List: {
                auto modeit = vars.find("mode");
                if (modeit == vars.end()) {
                    daemon.list_nodes();
                } else if (*modeit == "ring") {
                    cout << "[daemon] Listing node " << vars.at("node") << endl;
                    daemon.list_ring(vars.at("node"));
                } else if (*modeit == "*") {
                    cout << "[daemon] Listing all nodes" << endl;
                    daemon.list_all_nodes();
                }
            }   break;
            case Commands::Join:
                daemon.join(vars.at("node"));
                break;
            case Commands::Depart:
                daemon.depart(vars.at("node"));
                break;
            case Commands::Query:
                daemon.query(vars.at("key"));
                break;
            case Commands::Insert:
                daemon.insert(vars.at("key"), vars.at("value"));
                break;
            case Commands::Delete:
                daemon.remove(vars.at("key"));
                break;
            default:
                it.recycle_message();
                break;
        }
    } catch (const exception& e) {
        cerr << "Could not process client request:" << endl;
        print_exception(e);
    }
}
catch (const exception& e) {
    cerr << "The daemon crashed:" << endl;
    print_exception(e);
    exit(EXIT_FAILURE);
}
