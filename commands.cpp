#include "commands.hpp"
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <unordered_map>
#include <iostream>

using namespace std;

static const unordered_map<string, Commands> command_enums{
    {"help",        Commands::Help},
    {"version",     Commands::Version},
    {"status",      Commands::Status},
    {"start",       Commands::Start},
    {"terminate",   Commands::Terminate},
    {"list",        Commands::List},
    {"join",        Commands::Join},
    {"depart",      Commands::Depart},
    {"query",       Commands::Query},
    {"insert",      Commands::Insert},
    {"delete",      Commands::Delete}};

/// Get the corresponding command enumeration
/// for a given command name.
Commands to_command_enum(string s) try {
    return command_enums.at(s);
}   catch (const out_of_range&) {
    throw out_of_range("Command name '" + s + "' has no matching enumeration");
}

/// Get a vector with all the supported command names
/// (sorted by enum id).
vector<string> all_command_names(){
    static bool cached(false);
    static vector<string> names(command_enums.size());
    if (cached) return names;
    for (auto&& p: command_enums) names[static_cast<int>(p.second)] = p.first;
    return (cached = true), names;
}
