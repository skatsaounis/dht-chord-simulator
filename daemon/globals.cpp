#include "globals.hpp"
#include <stdexcept>

using namespace std;

const string socket_path("/var/run/dsemu/dsock");

string to_string(ConsistencyTypes c) {
    switch(c) {
        case ConsistencyTypes::Linear:
            return "linear";
        case ConsistencyTypes::Eventual:
            return "eventual";
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
