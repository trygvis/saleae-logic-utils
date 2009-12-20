#include "common.h"

Log log("to_event");

const int buf_size = 1024;

int main(int argc, char* argv[]) {
    char buf[buf_size];

    int in_file = STDIN_FILENO;
    int out_file = STDOUT_FILENO;

    ssize_t n_read;
    while((n_read = read(in_file, buf, buf_size)) > 0) {
        log.info("Read %d bytes", n_read);
    }

    if(n_read == 0) {
        log.debug("EOF");
        return EXIT_SUCCESS;
    }

    log.error("Error while reading.");
    return EXIT_FAILURE;
}
