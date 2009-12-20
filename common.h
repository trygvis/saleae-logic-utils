#include <string>
#include <iostream>

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

class SampleRate {
    public:
        const uint32_t value;
        const std::string text;

        static int count();
        static SampleRate* parse(const char* str);
        static SampleRate& get(int i);
};

class ByteOutStream {
    public:
        ByteOutStream(SampleRate* sampleRate, std::ostream& stream);

        void write(uint8_t byte);
        void write(uint8_t* byte, std::streamsize size);

    private:
        std::ostream& stream;
};

class ByteInStream {
    public:
        ByteInStream(std::istream& stream);

        uint8_t read();

    private:
        std::istream& stream;
};
