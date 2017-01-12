#ifndef _DSEMU_OPTIONS_HPP_
#define _DSEMU_OPTIONS_HPP_

#include "commands.hpp"

class Options
{
    enum class TargetTypes: unsigned {
        None, Daemon, Node
    };

    Commands _m_command;
    unsigned _m_node;
    TargetTypes _m_target_type = TargetTypes::None;

public:
    bool was_node_specified() const noexcept;

    Commands command() const;
    unsigned node() const;

    void parse(int argc, char** argv);
};

#endif  // _DSEMU_OPTIONS_HPP_
