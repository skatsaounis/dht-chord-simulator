#ifndef _DSEMU_GLOBALS_HPP_
#define _DSEMU_GLOBALS_HPP_

#include <string>

extern const std::string socket_path;

enum class ConsistencyTypes {
    Linear, Eventual
};

std::string to_string(ConsistencyTypes);
ConsistencyTypes to_consistency_enum(const std::string&);

#endif
