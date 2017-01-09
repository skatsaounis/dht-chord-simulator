#ifndef _DSEMU_COMMANDS_HPP_
#define _DSEMU_COMMANDS_HPP_

#include <vector>
#include <string>

/// Enumeration of all the commands that
/// can be executed via the cli.
enum class Commands
{
    Help,
    Version,
    Status,
    Start,
    Terminate,
    List,
    Join,
    Depart,
    Query,
    Insert,
    Delete
};

// Text utilities
Commands to_command_enum(std::string);
std::vector<std::string> all_command_names();

#endif  // _DSEMU_COMMANDS_HPP_
