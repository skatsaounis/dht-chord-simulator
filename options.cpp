#include "options.hpp"
#include "commands.hpp"
#include <tclap/CmdLine.h>

using namespace std;
using namespace TCLAP;

Commands Options::command() {
    return _m_command;
}

void Options::parse(int argc, char** argv) try {
    CmdLine cmd("Distributed Systems Emulator", ' ', "0.0.1");
    auto commands = all_command_names();
    ValuesConstraint<string> allowed_commands(commands);
    UnlabeledValueArg<string> command_arg("command",
        "Command to be executed by the Distributed Systems Emulator Daemon (dsemud).\n"
        "Use 'dsemu help <command>' to see usage for each command.",
        true, "help", &allowed_commands, cmd);
    UnlabeledMultiArg<string> parameter_args("parameters",
        "Parameters passed to the executed command.",
        false, "parameters", cmd, true);
    cmd.parse(argc, argv);
    _m_command = to_command_enum(command_arg.getValue());
    if (command() == Commands::Help) cmd.getOutput()->usage(cmd);
}
catch (const ArgException& e) {
    throw;
}
