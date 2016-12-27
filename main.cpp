#include "options.hpp"
#include <stdexcept>
#include <iostream>
#include <string>

using namespace std;

static void print_exception(const exception&, int level = 0);

int main(int argc, char** argv) try {
    Options options;
    options.parse(argc, argv);
    // switch(options.command()) {}
    // ...
}
catch (const exception& e) {
    cerr << "The program crashed:" << endl;  // TODO: Different message on daemon crash
    print_exception(e);
    exit(EXIT_FAILURE);
}

static void print_exception(const exception& e, int level) try {
    cerr << string(level, ' ') << e.what() << endl;
    rethrow_if_nested(e);
} catch(const exception& e2) {
    print_exception(e2, level + 1);
}
