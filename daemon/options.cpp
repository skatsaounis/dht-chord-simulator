#include "options.hpp"
#include "commands.hpp"
#include <tclap/CmdLine.h>
#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace TCLAP;

bool Options::was_node_specified() const noexcept {
    return _m_was_node_specified;
}

Commands Options::command() const noexcept {
    return _m_command;
}

unsigned Options::node() const try {
    if (!_m_was_node_specified) throw runtime_error("No node was specified");
    return _m_node;
} catch (const exception&) {
    throw_with_nested(runtime_error("While requesting node number from parsed options"));
}

string Options::key() const noexcept {
    return _m_key;
}

string Options::value() const noexcept {
    return _m_value;
}

unsigned Options::n_replicas() const noexcept {
    return _m_n_replicas;
}

ConsistencyTypes Options::consistency() const noexcept {
    return _m_consistency;
}

string Options::list_mode() const noexcept {
    return _m_list_mode;
}

void Options::parse(int argc, char** argv) try {
    CmdLine cmd("Distributed Systems Emulator", ' ', "0.0.1", false);
    cmd.setExceptionHandling(false);
    auto commands = all_command_names();
    ValuesConstraint<string> allowed_commands(commands);
    UnlabeledValueArg<string> command_arg("command",
        "Command to be executed by the Distributed Systems Emulator Daemon.\n"
        "Use 'dsemu <command> --help' to see usage for each command.",
        true, "help", &allowed_commands, cmd);
    UnlabeledMultiArg<string> parameter_args("parameters",
        "Parameters passed to the command to be executed.",
        false, "parameters", cmd, true);
    cmd.parse(argc, argv);
    _m_command = to_command_enum(command_arg.getValue());
    auto parameters = parameter_args.getValue();
    if (!parameters.empty()) parameters.emplace(parameters.begin(), "dsemu " + command_arg.getValue());
    switch (command()) {
        case Commands::Help:
            cmd.getOutput()->usage(cmd);
            exit(EXIT_SUCCESS);
        case Commands::Version:
            cmd.getOutput()->version(cmd);
            exit(EXIT_SUCCESS);
        case Commands::Start:
            if (parameters.empty())
                _m_was_node_specified = false;
            else {
                CmdLine params("Distributed Systems Emulator: Start command");
                params.setExceptionHandling(false);
                ValueArg<unsigned> node_id("n", "node", "ID of the node to initialize", true, 0, "Integer", params);
                ValueArg<unsigned> n_replicas("r", "replicas", "Number of replicas of the DHT values", false, 1, "Integer", params);
                vector<string> consistency_types{"linear", "eventual"};
                ValuesConstraint<string> allowed_consistency(consistency_types);
                ValueArg<string> consistency("c", "consistency", "Type of consistency", false, "linear", &allowed_consistency, params);
                params.parse(parameters);
                _m_was_node_specified = true;
                _m_node = node_id.getValue();
                _m_n_replicas = n_replicas.getValue();
                _m_consistency = to_consistency_enum(consistency.getValue());
            }
            break;
        case Commands::Terminate:
            if (parameters.empty())
                _m_was_node_specified = false;
            else {
                CmdLine params("Distributed Systems Emulator: Terminate command");
                params.setExceptionHandling(false);
                ValueArg<unsigned> node_id("n", "node", "ID of the node to terminate", true, 0, "Integer", params);
                params.parse(parameters);
                _m_was_node_specified = true;
                _m_node = node_id.getValue();
            }
            break;
        case Commands::List:
            if (parameters.empty())
                _m_list_mode = "simple";
            else {
                CmdLine params("Distributed Systems Emulator: List command");
                params.setExceptionHandling(false);
                vector<string> list_modes{"simple", "ring"};
                ValuesConstraint<string> allowed_list_modes(list_modes);
                UnlabeledValueArg<string> list_mode("mode", "Mode of list operation", false, "simple", &allowed_list_modes, params);
                ValueArg<unsigned> node_id("n", "node", "ID of the node to list information", false, 0, "Integer", params);
                params.parse(parameters);
                _m_was_node_specified = node_id.isSet();
                _m_list_mode = list_mode.getValue();
                _m_node = node_id.getValue();
            }
            break;
        case Commands::Join: {
                CmdLine params("Distributed Systems Emulator: Join command");
                params.setExceptionHandling(false);
                ValueArg<unsigned> node_id("n", "node", "ID of the node to join", true, 0, "Integer", params);
                params.parse(parameters);
                _m_node = node_id.getValue();
                _m_was_node_specified = true;
            }   break;
        case Commands::Depart: {
                CmdLine params("Distributed Systems Emulator: Depart command");
                params.setExceptionHandling(false);
                ValueArg<unsigned> node_id("n", "node", "ID of the departing node", true, 0, "Integer", params);
                params.parse(parameters);
                _m_node = node_id.getValue();
                _m_was_node_specified = true;
            }   break;
        case Commands::Query: {
                CmdLine params("Distributed Systems Emulator: Query command");
                params.setExceptionHandling(false);
                ValueArg<string> key("k", "key", "Key to query from the chord", true, 0, "String", params);
                params.parse(parameters);
                _m_key = key.getValue();
            }   break;
        case Commands::Insert: {
                CmdLine params("Distributed Systems Emulator: Insert command");
                params.setExceptionHandling(false);
                ValueArg<string> key("k", "key", "Key to be added to the chord", true, 0, "String", params);
                ValueArg<string> value("v", "value", "Value to be associated with the key", true, 0, "String", params);
                params.parse(parameters);
                _m_key = key.getValue();
                _m_value = value.getValue();
            }   break;
        case Commands::Delete: {
                CmdLine params("Distributed Systems Emulator: Delete command");
                params.setExceptionHandling(false);
                ValueArg<string> key("k", "key", "Key to delete from the chord", true, 0, "String", params);
                params.parse(parameters);
                _m_key = key.getValue();
            }   break;
        default:
            break;
    }
}
catch (const ArgException& e) {
    cerr << (e.argId() == "undefined" ? e.error() : e.what()) << endl;
    exit(EXIT_FAILURE);
}
catch (const exception&) {
    throw_with_nested(runtime_error("While parsing command line parameters"));
}
