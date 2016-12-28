#ifndef _DSEMU_OPTIONS_HPP_
#define _DSEMU_OPTIONS_HPP_

#include "commands.hpp"

class Options
{
    Commands _m_command;

public:
    Commands command();
    void parse(int argc, char** argv);
};

#endif  // _DSEMU_OPTIONS_HPP_
