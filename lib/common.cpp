#include "common.h"

#include <iostream>
#include <errno.h>

using namespace std;
using namespace Logic;

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
// LogicException
// *****************************************************************************

LogicException::LogicException(const char* fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    (void) vsnprintf(message, sizeof(message), fmt, ap);
    va_end(ap);
};

// *****************************************************************************
// EventTime
// *****************************************************************************

char* EventTime::to_str(char* buf, size_t size) const {
    double v;
    const char* unit;
    if(value >= 10000000000ll) {
        unit = "ms";
        v = value / 1000000000.0;
    }
    else if(value >= 10000000) {
        unit = "us";
        v = value / 1000000.0;
    }
    else if(value >= 10000) {
        unit = "ns";
        v = value / 1000.0;
    }
    else if(value == 0){
        unit = "s";
        v = 0;
    }
    else {
        unit = "ps";
        v = value;
    }
//    fprintf(stderr, "value=%llu, v=%f, unit=%s\n", value, v, unit);
    snprintf(buf, size, "%.2f%s", v, unit);
    return buf;
}

// *****************************************************************************
// SampleRate
// *****************************************************************************

static const int numberOfAvailableSampleRates = 13;

int SampleRate::count() {
    return numberOfAvailableSampleRates;
}

const SampleRate* SampleRate::sampleRate24MHz = new SampleRate(24000000, "24MHz");
const SampleRate* SampleRate::sampleRate16MHz = new SampleRate(16000000, "16MHz");
const SampleRate* SampleRate::sampleRate12MHz = new SampleRate(12000000, "12MHz");
const SampleRate* SampleRate::sampleRate8MHz = new SampleRate(8000000, "8MHz");
const SampleRate* SampleRate::sampleRate4MHz = new SampleRate(4000000, "4MHz");
const SampleRate* SampleRate::sampleRate2MHz = new SampleRate(2000000, "2MHz");
const SampleRate* SampleRate::sampleRate1MHz = new SampleRate(1000000, "1MHz");
const SampleRate* SampleRate::sampleRate500kHz = new SampleRate(500000, "500kHz");
const SampleRate* SampleRate::sampleRate250kHz = new SampleRate(250000, "250kHz");
const SampleRate* SampleRate::sampleRate200kHz = new SampleRate(200000, "200kHz");
const SampleRate* SampleRate::sampleRate100kHz = new SampleRate(100000, "100kHz");
const SampleRate* SampleRate::sampleRate50kHz = new SampleRate(50000, "50kHz");
const SampleRate* SampleRate::sampleRate25kHz = new SampleRate(25000, "25kHz");

const SampleRate* SampleRate::getAvailableSampleRate(int i) {
    static const SampleRate* availableSampleRates[numberOfAvailableSampleRates] = {
        sampleRate24MHz,
        sampleRate16MHz,
        sampleRate12MHz,
        sampleRate8MHz,
        sampleRate4MHz,
        sampleRate2MHz,
        sampleRate1MHz,
        sampleRate500kHz,
        sampleRate250kHz,
        sampleRate200kHz,
        sampleRate100kHz,
        sampleRate50kHz,
        sampleRate25kHz,
    };

    return availableSampleRates[i];
}

const SampleRate* SampleRate::fromBytes(uint32_t bytes) {
    uint32_t freq = ntohl(bytes);
    for(int i = 0; i < count(); i++) {
        const SampleRate* s = getAvailableSampleRate(i);
        if(freq == s->freq){
            return s;
        }
    }

    throw LogicException("SampleRate: Invalid sample rate: %u", freq);
}

const SampleRate* SampleRate::parse(const char* str) {
    char* end;
    unsigned int freq = strtol(str, &end, 10);

    if(freq > 0) {
        for(int i = 0; i < count(); i++) {
            const SampleRate* s = getAvailableSampleRate(i);
            if(freq == s->freq){
                return s;
            }
        }
    }

    for(int i = 0; i < count(); i++) {
        const SampleRate* s = getAvailableSampleRate(i);
        if(strcmp(str, s->text) == 0){
            return s;
        }
    }

    return NULL;
}

// *****************************************************************************
// EventTimer
// *****************************************************************************

void EventTimer::increment() {
    current_time += sampleRate.interval;
}

EventTime EventTimer::currentTime() const {
    return EventTime(current_time);
}

// *****************************************************************************
// ByteStreamHeader
// *****************************************************************************

static const uint32_t BYTE_STREAM_MAGIC = 's' << 24 & 't' << 16 & 'r' << 8 & 'm';
static const uint32_t EVENT_STREAM_MAGIC = 'e' << 24 & 'v' << 16 & 'n' << 8 & 't';

// TODO: At some point, decide how many bytes this header has to be and add padding.
struct ByteStreamHeader {
    ByteStreamHeader(const SampleRate* sampleRate) : _magic(BYTE_STREAM_MAGIC), _sampleRate(htonl(sampleRate->freq)) {}

    const SampleRate* sampleRate() {
        return SampleRate::fromBytes(_sampleRate);
    }

    static ByteStreamHeader read(istream& stream) {
        ByteStreamHeader header;
        stream.read((char*)&header, sizeof(header));
        if(stream.eof()) {
            throw LogicException("ByteStreamHeader: EOF.");
        }

        if(header._magic != BYTE_STREAM_MAGIC) {
            throw LogicException("ByteStreamHeader: Invalid magic: 0x%04x", header._magic);
        }

        return header;
    }

    private:
        uint32_t _magic;
        uint32_t _sampleRate;
        ByteStreamHeader() {}
};

// *****************************************************************************
// ByteOutStream
// *****************************************************************************

ByteOutStream::ByteOutStream(const SampleRate* sampleRate, ostream& stream) : stream(stream) {
    ByteStreamHeader header = ByteStreamHeader(sampleRate);
    stream.exceptions(ios::eofbit | ios::failbit | ios::badbit);
    stream.write((const char*)&header, sizeof(header));
    stream.flush();
}

bool ByteOutStream::write(const uint8_t byte) {
    return write(&byte, 1);
}

bool ByteOutStream::write(const uint8_t* bytes, std::streamsize size) {
    stream.write((const char*)bytes, size);

    return stream.good();
}

// *****************************************************************************
// ByteInStream
// *****************************************************************************

ByteInStream::ByteInStream(std::istream& stream) : stream(stream) {
    ByteStreamHeader header = ByteStreamHeader::read(stream);
    _sampleRate = new SampleRate(*header.sampleRate());
    this->eventTimer = new EventTimer(*_sampleRate);
}

uint8_t ByteInStream::read() {
    char byte;
    stream.read(&byte, 1);
    if(stream.good()) {
        eventTimer->increment();
    }
}

bool ByteInStream::isOk() {
    return stream.good();
}

EventTime ByteInStream::currentTime() const {
    return eventTimer->currentTime();
}

const SampleRate* ByteInStream::sampleRate() const {
    return _sampleRate;
}

// *****************************************************************************
// EventOutStream
// *****************************************************************************

EventOutStream::EventOutStream(std::ostream& stream) : stream(stream) {
}

bool EventOutStream::write(LogicEvent event) {
}
