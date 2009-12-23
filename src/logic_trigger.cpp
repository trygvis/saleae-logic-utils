#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <pthread.h>
#include <unistd.h>

#include "LogicInterface.h"

using namespace std;

/*
 TODO: Support strings when identifying sample rates ("24M")
 TODO: Support colors when identifying bits
 0: Black
 1: Brown
 2: red
 3: orange
 4: yellow
 5: green
 6: blue
 7: violet
*/

LogicInterface gLogicInterface;
vector<unsigned int> gLogicIds;
unsigned int sampleRateHz;

void OnConnect(unsigned int logic_id, void* user_data);
void OnDisconnect(unsigned int logic_id, void* user_data);
void OnReadData( unsigned int logic_id, unsigned char* data, unsigned int data_length, void* user_data );
//void OnWriteData( unsigned int logic_id, unsigned char* data, unsigned int data_length, void* user_data );
void OnError(unsigned int logic_id, void* user_data);

pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

unsigned int logicId;

class Trigger {
    public:
        Trigger(string name) : name(name) { }
        const string name;

    private:
        static map<string, Trigger*> trigger_types;
};

Trigger TRIGGER_HIGH = Trigger("high");
Trigger TRIGGER_LOW = Trigger("low");
Trigger TRIGGER_RISING = Trigger("rising");
Trigger TRIGGER_FALLING = Trigger("falling");

Trigger* trigger = NULL;

const char *me = "logic_trigger";

void usage(char* msg) {
    fprintf(stderr, "usage: %s [-t <trigger>] [-b <bit>]\n", me);
    fprintf(stderr, "\n");
    fprintf(stderr, " Sample rate can be one of: \n");
    fprintf(stderr, "  24000000 (24MHz)\n");
    fprintf(stderr, "  16000000 (16MHz)\n");
    fprintf(stderr, "  12000000 (12MHz)\n");
    fprintf(stderr, "   8000000 (8MHz)\n");
    fprintf(stderr, "   4000000 (4MHz)\n");
    fprintf(stderr, "   2000000 (2MHz)\n");
    fprintf(stderr, "   1000000 (1MHz)\n");
    fprintf(stderr, "    500000 (500kHz)\n");
    fprintf(stderr, "    250000 (250kHz)\n");
    fprintf(stderr, "    200000 (200kHz)\n");
    fprintf(stderr, "    100000 (100kHz)\n");
    fprintf(stderr, "     50000 (50kHz)\n");
    fprintf(stderr, "     25000 (25kHz)\n");
    fprintf(stderr, "\n");
    fprintf(stderr, " Trigger may be one of\n");
    fprintf(stderr, "  High\n");
    fprintf(stderr, "  Low\n");
    fprintf(stderr, "  Rising\n");
    fprintf(stderr, "  Level\n");
    fprintf(stderr, "\n");
    fprintf(stderr, " Bit is a number from 0 to 8\n");

    if(msg) {
        fprintf(stderr, "%s\n", msg);
    }
    exit(EXIT_FAILURE);
}

bool logic_parse_args(int argc, char *argv[]) {
    char c;
    while ((c = getopt (argc, argv, "t:r:b")) != -1) {
        switch(c) {
            case 'r': // Sample rate
                sampleRateHz = atoi(optarg);
                return true;
            case 't':   // trigger
                // "falling", "rising", "high", "low"
                return true;
            case 'b':   // trigger bit
                return true;
            default:
                usage(NULL);
                return false;
        }
    }
}

void logic_work()
{
}

void logic_close() {
}
