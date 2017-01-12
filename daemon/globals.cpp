#include "globals.hpp"

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
