#include <string>
#include <iostream>

class Log {
    public:
        Log(std::string program);

        void info(char* fmt, ...);
        void info(std::string message);

        void error(char* fmt, ...);
        void error(std::string message);

    private:
        void write(const char* message);
        const std::string program;
        std::ostream* stream;
};
