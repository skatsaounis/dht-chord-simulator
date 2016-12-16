#include "options.hpp"
#include <tclap/CmdLine.h>

using namespace std;
using namespace TCLAP;

void Options::parse(int argc, char** argv) try {
    CmdLine cmd("Distributed Systems Emulator", ' ', "0.0.1");
    vector<string> commands{"help", "status", "start", "terminate",
         "list", "join", "depart", "query", "insert", "delete"};
    ValuesConstraint<string> allowed_commands(commands);
    UnlabeledValueArg<string> command("command",
        "Command to be executed by the Distributed Systems Emulator Daemon (dsemud)",
        true, "help", &allowed_commands, cmd);
    UnlabeledMultiArg<string> parameters("parameters",
        "Parameters passed to the executed command",
        false, "list of parameters", cmd, true);
    cmd.parse(argc, argv);
    if (command.getValue() == "help") cmd.getOutput()->usage(cmd);
}
catch (const ArgException& e) {

}
