#include "common.h"

#include <iostream>

using namespace std;
using namespace Logic;

bool logic_parse_args(int argc, char *argv[]);
void logic_work();
void logic_close();

//extern const char* me;

extern Log log;

int main(int argc, char *argv[])
{
    if(!logic_parse_args(argc, argv)) {
        return EXIT_FAILURE;
    }

    try {
        logic_work();
    }
    catch(exception& e) {
        log.error("Uncaught exception: %s", e.what());
    }
    catch(LogicException& e) {
        log.error("Logic exception: %s", e.what());
    }
    catch(...) {
        log.error("Unknown uncaught exception.");
    }

    logic_close();
    return EXIT_SUCCESS;
}
