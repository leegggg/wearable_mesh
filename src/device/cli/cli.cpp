#include "cli.hpp"

Cli::Cli(HWCDC* serial, bool echo) {
    this->serial = serial;
    this->cmdBuffer = new CircularBuffer<char, CMD_BUFFER_SIZE>();
    this->ready = false;
    this->echo = echo;
}

Cli::~Cli() {
    delete this->cmdBuffer;
}

bool Cli::available(){
    while(this->serial->available()) {
        char c = this->serial->read();
        if(c == '\r'){
            c = '\n';
        }
        if (this->echo) {
            this->serial->print(c);
        }
        if (c == '\n') {
            this->cmdBuffer->push('\0');
            char* cmdArray = (char*)malloc(this->cmdBuffer->size());
            this->cmdBuffer->copyToArray(cmdArray);
            String cmd = String(cmdArray);
            free(cmdArray);
            this->cmd = cmd;
            this->ready = true;
            // clear buffer
            cmdBuffer->clear();
            this->serial->flush();
        } else {
            this->cmdBuffer->push(c);
        }
    }
    return this->ready;
}

String Cli::read() {
    this->ready = false;
    return this->cmd;
}



