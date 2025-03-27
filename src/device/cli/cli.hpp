#ifndef CLI_HPP
#define CLI_HPP

#include <Arduino.h>
#include <CircularBuffer.hpp>

#define CMD_BUFFER_SIZE 64

class Cli {
public:
    Cli(HWCDC* serial, bool echo);
    ~Cli();
    bool available();
    String read();
private:
    HWCDC* serial;
    CircularBuffer<char, CMD_BUFFER_SIZE>* cmdBuffer;
    bool ready;
    bool echo;
    String cmd;
};

#endif // CLI_HPP