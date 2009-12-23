#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "common.h"

using namespace Logic;

uint8_t* data;
size_t data_size;

Log log("pattern_generator");
ByteOutStream* out;

// TODO: generate patterns for misc protocols, i2c, rs232

void alloc_data(size_t size) {
    if((data = (uint8_t*)malloc(size)) == NULL) {
        log.error("Unable to allocate %d bytes.", size);
        exit(EXIT_FAILURE);
    }
    data_size = size;
}

void pattern1() {
    int size = 256;
    alloc_data(size);
    for(int i = 0; i < size; i++) {
        data[i] = i;
    }
}

const char* me = "pattern_generator";

const char* pattern_descriptions[] = {
    "Sequence of bytes from 0 to 255",
};

bool usage(const char* message) {
    int i;

    log.error("Usage: %s -r <sample rate> [1..%d]", me, sizeof(pattern_descriptions));
    log.error(" Patterns:", me);
    for(i = 0; i < sizeof(pattern_descriptions); i++) {
        log.error("  %d: %s", i, pattern_descriptions[i]);
    }

    if(message != NULL) {
        log.error(message);
    }

    return false;
}

const SampleRate* sampleRate = NULL;
int pattern;

bool logic_parse_args(int argc, char* argv[]) {
    const char* me = argv[0];
    char c;
    while ((c = getopt (argc, argv, "r:")) != -1) {
        switch(c) {
            case 'r': // Sample rate
                sampleRate = SampleRate::parse(optarg);
                break;
            case '?':
                log.error("woot? got '?'");
                return usage(NULL);
        }
    }
    argc -= optind;
    argv += optind;

    if(sampleRate == NULL) {
        return usage("Invalid sample rate, has to be one of the allowed values.");
    }

    if(argc != 1) {
        return usage("Exactly one pattern id has to be specified.");
    }

    pattern = atoi(argv[0]);

    return true;
}

void logic_work() {
    switch(pattern) {
        case 1:
            pattern1();
            break;
    }

    log.info("Writing %d bytes", data_size);
    out = new ByteOutStream(sampleRate, std::cout);
    out->write(data, data_size);
    free(data);
}

void logic_close() {
    delete out;
}
