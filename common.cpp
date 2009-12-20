#include "common.h"

#include <iostream>

using namespace std;

Log::Log(string program) : program(program), stream(&cout) {
}

void Log::info(char* fmt, ...) {
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

void Log::error(char* fmt, ...) {
    char p[1024];
    va_list ap;

    va_start(ap, fmt);
    (void) vsnprintf(p, 1024, fmt, ap);
    va_end(ap);
    stream->write(program.c_str(), program.size());
    stream->write(": ", 2);
    stream->write(p, strlen(p));
    write(p);
}

void Log::error(string message) {
    write(message.c_str());
}

void Log::write(const char* message) {
    stream->write(program.c_str(), program.size());
    stream->write(": ", 2);
    stream->write(message, strlen(message));
    stream->write("\n", 1);
    stream->flush();
}
