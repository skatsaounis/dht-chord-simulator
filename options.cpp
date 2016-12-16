#include "options.hpp"
#include <tclap/CmdLine.h>

using namespace std;
using namespace TCLAP;

void Options::parse(int argc, char** argv) try {
    CmdLine cmd("Distributed Systems Emulator", ' ', "0.0.1");
    vector<string> commands{"status", "start", "terminate", "help"};
    ValuesConstraint<string> allowed_commands(commands);
    UnlabeledValueArg<string> command("command",
        "Command to be executed by the Distributed Systems Emulator Daemon (dsemud)",
        true, "help", &allowed_commands, cmd);
    cmd.parse(argc, argv);
    if (command.getValue() == "help") cmd.getOutput()->usage(cmd);
}
catch (const ArgException& e) {

}
