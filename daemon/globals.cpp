#include "globals.hpp"
#include <stdexcept>

using namespace std;

const string socket_dir("/var/run/dsemu/");
const string socket_path(socket_dir + "dsock");

string to_string(ConsistencyTypes c) {
    switch(c) {
        case ConsistencyTypes::Linear:
            return "linear";
        case ConsistencyTypes::Eventual:
            return "eventual";
        default:
            throw runtime_error("No string found for consistency enum");
    }
}

ConsistencyTypes to_consistency_enum(const string& s) {
    if (s == "linear")
        return ConsistencyTypes::Linear;
    else if (s == "eventual")
        return ConsistencyTypes::Eventual;
    else
        throw runtime_error("No consistency enumeration for " + s);
}
