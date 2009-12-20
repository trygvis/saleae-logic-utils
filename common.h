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
