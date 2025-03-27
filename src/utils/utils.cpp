#include "utils.hpp"

#include <Arduino.h>
#include "../global.hpp"

bool runBootDelay(int bootDelaySeconds, int pinOnBoardLed){
    pinMode(pinOnBoardLed, OUTPUT);
    for(int i = 0; i < bootDelaySeconds; i++) {
        delay(1000);
        // blink on board led
        digitalWrite(pinOnBoardLed, i % 2 == 0 ? HIGH : LOW);
    }
    // fast blink 3 times for boot prosess
    for(int i = 0; i < 6; i++) {
        digitalWrite(pinOnBoardLed, i % 2 == 0 ? HIGH : LOW);
        delay(100);
    }
    return true;
}