#ifndef _DSEMU_OPTIONS_HPP_
#define _DSEMU_OPTIONS_HPP_

#include "globals.hpp"
#include "commands.hpp"

class Options
{
    enum class TargetTypes: unsigned {
        None, Daemon, Node
    };

    Commands _m_command;
    TargetTypes _m_target_type = TargetTypes::None;
    unsigned _m_node;
    unsigned _m_n_replicas = 1;
    ConsistencyTypes _m_consistency = ConsistencyTypes::Linear;

public:
    bool was_node_specified() const noexcept;

    Commands command() const;
    unsigned node() const;
    unsigned n_replicas() const noexcept;
    ConsistencyTypes consistency() const noexcept;

    void parse(int argc, char** argv);
};

#endif  // _DSEMU_OPTIONS_HPP_
