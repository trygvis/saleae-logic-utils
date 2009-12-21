#include "common.h"

#include <iostream>
#include <errno.h>

using namespace std;

extern Log log;

Log::Log(const char* program) : program(program), stream(&cerr) {
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

// *****************************************************************************
// SampleRate
// *****************************************************************************

static const int numberOfAllowedSampleRates = 13;

static SampleRate allowedSampleRates[numberOfAllowedSampleRates] = {
    {24000000, "24MHz"},
    {16000000, "16MHz"},
    {12000000, "12MHz"},
    {8000000, "8MHz"},
    {4000000, "4MHz"},
    {2000000, "2MHz"},
    {1000000, "1MHz"},
    {500000, "500kHz"},
    {250000, "250kHz"},
    {200000, "200kHz"},
    {100000, "100kHz"},
    {50000, "50kHz"},
    {25000, "25kHz"}
};

int SampleRate::count() {
    return numberOfAllowedSampleRates;
}

SampleRate& SampleRate::get(int i) {
    return allowedSampleRates[i];
}

SampleRate* SampleRate::parse(const char* str) {
    char* end;
    unsigned int freq = strtol(str, &end, 10);

    if(freq == 0) {
        return NULL;
    }

    for(int i = 0; i < count(); i++) {
        SampleRate* s = &allowedSampleRates[i];
        if(freq == s->freq){
            return s;
        }
    }

    return NULL;
}

// *****************************************************************************
// ByteOutStream
// *****************************************************************************

/*
 * Creates a byte stream in network order like:
 * Header:
 *   uint32_t: sample rate
 *   uint32_t: reserved
 *   uint32_t: reserved
 *   uint32_t: reserved
 * Body:
 *   uint_8: byte n
 */

ByteOutStream::ByteOutStream(SampleRate* sampleRate, ostream& s) : stream(s) {
    uint32_t header[] = {htonl(sampleRate->freq), 0, 0, 0};
    s.exceptions(ios::eofbit | ios::failbit | ios::badbit);
    s.write((const char*)&header, 16);
    s.flush();
}

bool ByteOutStream::write(const uint8_t byte) {
    return write(&byte, 1);
}

bool ByteOutStream::write(const uint8_t* bytes, std::streamsize size) {
    stream.write((const char*)bytes, size);

    return stream.good();
}
