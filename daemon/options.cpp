#include "options.hpp"
#include "commands.hpp"
#include <tclap/CmdLine.h>
#include <cstdlib>
#include <stdexcept>

using namespace std;
using namespace TCLAP;

bool Options::was_node_specified() const noexcept {
    return _m_target_type == TargetTypes::Node;
}

Commands Options::command() const {
    return _m_command;
}

unsigned Options::node() const try {
    if (_m_target_type != TargetTypes::Node) throw runtime_error("No node was specified");
    return _m_node;
} catch (const exception&) {
    throw_with_nested(runtime_error("While requesting node number from parsed options"));
}

unsigned Options::n_replicas() const noexcept {
    return _m_n_replicas;
}

ConsistencyTypes Options::consistency() const noexcept {
    return _m_consistency;
}

void Options::parse(int argc, char** argv) try {
    CmdLine cmd("Distributed Systems Emulator", ' ', "0.0.1", false);
    auto commands = all_command_names();
    ValuesConstraint<string> allowed_commands(commands);
    UnlabeledValueArg<string> command_arg("command",
        "Command to be executed by the Distributed Systems Emulator Daemon (dsemud).\n"
        "Use 'dsemu help <command>' to see usage for each command.",
        true, "help", &allowed_commands, cmd);
    UnlabeledMultiArg<string> parameter_args("parameters",
        "Parameters passed to the command to be executed.",
        false, "parameters", cmd, true);
    cmd.parse(argc, argv);
    _m_command = to_command_enum(command_arg.getValue());
    switch (command()) {
        case Commands::Help:
            cmd.getOutput()->usage(cmd);
            exit(EXIT_SUCCESS);
        case Commands::Version:
            cmd.getOutput()->version(cmd);
            exit(EXIT_SUCCESS);
        default:
            break;
        //TODO: parse 'start' arguments
    }
}
catch (const exception&) {
    throw_with_nested(runtime_error("While parsing command line parameters"));
}
