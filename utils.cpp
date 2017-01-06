#include "utils.hpp"
#include <iostream>

using namespace std;

void print_exception(const exception& e, int level) try {
    cerr << string(level, ' ') << e.what() << endl;
    rethrow_if_nested(e);
} catch(const exception& e2) {
    print_exception(e2, level + 1);
}
