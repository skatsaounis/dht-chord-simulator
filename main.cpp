#include "options.hpp"
#include <stdexcept>

using namespace std;

int main(int argc, char** argv) try {
    Options options;
    options.parse(argc, argv);
    // switch(options.command()) {}
    // ...
}
catch (const exception& e) {

}
