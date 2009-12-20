#include "common.h"

#include <iostream>
#include <errno.h>

using namespace std;

Log::Log(const char* program) : program(program), stream(&cout) {
}

Log::Log(const char* program, bool flip) : program(program), stream(flip ? &cout : &cerr) {
}

void Log::debug(const char* fmt, ...) {
    char p[1024];
    va_list ap;

    va_start(ap, fmt);
    (void) vsnprintf(p, 1024, fmt, ap);
    va_end(ap);
    write(p);
}

void Log::debug(string message) {
    write(message.c_str());
}

void Log::info(const char* fmt, ...) {
    char p[1024];
    va_list ap;

    va_start(ap, fmt);
    (void) vsnprintf(p, 1024, fmt, ap);
    va_end(ap);
    write(p);
}

void Log::info(string message) {
    write(message.c_str());
}

void Log::error(const char* fmt, ...) {
    char p[1024];
    va_list ap;

    va_start(ap, fmt);
    (void) vsnprintf(p, 1024, fmt, ap);
    va_end(ap);
    write(p);
}

void Log::error(string message) {
    write(message.c_str());
}

void Log::perror(const char* fmt, ...) {
    char p[1024];
    va_list ap;

    va_start(ap, fmt);
    (void) vsnprintf(p, 1024, fmt, ap);
    va_end(ap);

    write(strerror(errno));
    write(p);
}

void Log::perror(string message) {
    write(message.c_str());
}

void Log::write(const char* message) {
    stream->write(program, strlen(program));
    stream->write(": ", 2);
    stream->write(message, strlen(message));
    stream->write("\n", 1);
    stream->flush();
}
