#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "common.h"

uint8_t* data;
size_t data_size;

Log* log;

// TODO: generate patterns for misc protocols, i2c, rs232

void alloc_data(size_t size) {
    if((data = (uint8_t*)malloc(size)) == NULL) {
        log->error("Unable to allocate %d bytes.", size);
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

int usage(char* me) {
    log->error("Usage: %0 [1..1]\n", me);
    return EXIT_FAILURE;
}

int main(int argc, char* argv[]) {
    int out_file = STDOUT_FILENO;

    log = new Log(argv[0]);
    if(argc != 2) {
        return usage(argv[0]);
    }

    switch(atoi(argv[1])) {
        case 1:
            pattern1();
            break;
        default:
            return usage(argv[0]);
    }

    log->info("Writing %d bytes", data_size);

    while(data_size > 0) {
        ssize_t bytes_written = write(out_file, data, data_size);

        if(bytes_written == -1) {
            log->perror("Error while writing out bytes");
            return EXIT_FAILURE;
        }
        data_size -= bytes_written;
    }
    free(data);

    return EXIT_SUCCESS;
}
