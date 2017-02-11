#ifndef _DSEMU_OPTIONS_HPP_
#define _DSEMU_OPTIONS_HPP_

#include "globals.hpp"
#include "commands.hpp"

class Options
{
    Commands _m_command;
    bool _m_was_node_specified = false;
    bool _m_verbose = false;
    unsigned _m_node;
    unsigned _m_n_replicas = 1;
    ConsistencyTypes _m_consistency = ConsistencyTypes::Linear;
    std::string _m_list_mode;
    std::string _m_key;
    std::string _m_value;

public:
    bool was_node_specified() const noexcept;
    bool verbose() const noexcept;

    Commands command() const noexcept;
    unsigned node() const;
    unsigned n_replicas() const noexcept;
    ConsistencyTypes consistency() const noexcept;
    std::string list_mode() const noexcept;
    std::string key() const noexcept;
    std::string value() const noexcept;

    void parse(int argc, char** argv);
};

#endif  // _DSEMU_OPTIONS_HPP_
