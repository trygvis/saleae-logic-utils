#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <iostream>
#include <stdint.h>

namespace Logic {

class Log {
    public:
        Log(const char* program);
        Log(const char* program, bool flip);

        void debug(const char* fmt, ...);
        void debug(std::string message);

        void info(const char* fmt, ...);
        void info(std::string message);

        void error(const char* fmt, ...);
        void error(std::string message);

        void perror(const char* fmt, ...);
        void perror(std::string message);

    private:
        void write(const char* message);
        const char* program;
        std::ostream* stream;
};

// *****************************************************************************
// Data Types
// *****************************************************************************

struct EventTime {
    const long long value; // in pico seconds
    EventTime(const long long value) : value(value) {};

    char* to_str(char* buf, size_t size) const;
};

class LogicEvent {
    public:
        const EventTime& time;
        const uint8_t state;

        LogicEvent(const EventTime& time, const uint8_t state) : time(time), state(state) {};
};

class SampleRate {
    public:
        const uint32_t freq;
        const long long interval; // how long time a single sample represents
        const char* text;

        static int count();
        static const SampleRate* fromBytes(uint32_t bytes);
        static const SampleRate* parse(const char* str);
        static const SampleRate* getAvailableSampleRate(int i);

        static const SampleRate* sampleRate24MHz;
        static const SampleRate* sampleRate16MHz;
        static const SampleRate* sampleRate12MHz;
        static const SampleRate* sampleRate8MHz;
        static const SampleRate* sampleRate4MHz;
        static const SampleRate* sampleRate2MHz;
        static const SampleRate* sampleRate1MHz;
        static const SampleRate* sampleRate500kHz;
        static const SampleRate* sampleRate250kHz;
        static const SampleRate* sampleRate200kHz;
        static const SampleRate* sampleRate100kHz;
        static const SampleRate* sampleRate50kHz;
        static const SampleRate* sampleRate25kHz;

    private:
        SampleRate(const uint32_t freq, const char* text) : freq(freq), text(text), interval(1.0 / ((double)freq) * 1000 * 1000 * 1000 * 1000) {};
};

/* 1kHz = 1ms
 * 1MHz = 1us
 * 1GHz = 1ns
 * 1THz = 1ps
 */

class EventTimer {
    public:
        EventTimer(SampleRate& sampleRate) : sampleRate(sampleRate), current_time(0) {};

        void increment();
        EventTime currentTime() const;

    private:
        const SampleRate& sampleRate;
        long long current_time; // in pico seconds
};

class LogicException {
    public:
      LogicException(const char* fmt, ...);
    
      const char* what() const { return message; }
    private:
      char message[1000];
};

// *****************************************************************************
// Services
// *****************************************************************************

class ByteOutStream {
    public:
        ByteOutStream(const SampleRate* sampleRate, std::ostream& stream);

        bool write(const uint8_t byte);
        bool write(const uint8_t* byte, std::streamsize size);

    private:
        std::ostream& stream;
};

class ByteInStream {
    public:
        ByteInStream(std::istream& stream);

        uint8_t read();
        bool isOk();
        EventTime currentTime() const;

        const SampleRate* sampleRate() const;

    private:
        std::istream& stream;
        SampleRate* _sampleRate;
        EventTimer* eventTimer;
};

class EventOutStream {
    public:
        EventOutStream(std::ostream& stream);

        bool write(LogicEvent event);

    private:
        std::ostream& stream;
};

}; // namespace Logic

#endif
