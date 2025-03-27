#include "CliService.hpp"

CliService::CliService(Cli* cli) {
    this->cli = cli;
}

void CliService::setup() {
}

void CliService::loop() {
    if (cli != NULL && cli->available()) {
        String cmd = cli->read();
        String response = "";
        if(String("AT+RST").equals(cmd)) {
            ESP.restart();
        }
        if(cmd.startsWith("AT+UCI")) {
            response = uci->handleATCommand(cmd);
        }
        Serial.println(response);
    }
}