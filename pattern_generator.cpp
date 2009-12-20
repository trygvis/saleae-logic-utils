#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "common.h"

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
    alloc_data(10);
    for(int i = 0; i < 10; i+=2) {
        data[i] = 0x00;
        data[i + 1] = 0x01;
    }
}

int usage(const char* me, const char* message) {
    log.error("Usage: %0 [1..1]\n", me);

    if(message != NULL) {
        log.error(message);
    }

    return EXIT_FAILURE;
}

int main(int argc, char* argv[]) {
    const char* me = argv[0];
    char c;
    SampleRate* sampleRate = NULL;
    while ((c = getopt (argc, argv, "r:")) != -1) {
        switch(c) {
            case 'r': // Sample rate
                sampleRate = SampleRate::parse(optarg);
                break;
            case '?':
                log.error("woot? got '?'");
                return usage(me, NULL);
        }
    }
    argc -= optind;
    argv += optind;

    if(sampleRate == NULL) {
        return usage(me, "Invalid sample rate, has to be one of the allowed values.");
    }

    if(argc != 1) {
        return usage(me, "Exactly one pattern id has to be specified.");
    }

    switch(atoi(argv[0])) {
        case 1:
            pattern1();
            break;
        default:
            return usage(me, "Invalid pattern id.");
    }

    log.info("Writing %d bytes", data_size);
    out = new ByteOutStream(sampleRate, std::cout);
    out->write(data, data_size);
    free(data);

    delete out;

    return EXIT_SUCCESS;
}
