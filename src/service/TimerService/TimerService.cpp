#include "TimerService.hpp" 


TimerService::TimerService(IService* service, unsigned long interval) {
    this->service = service;
    this->interval = interval;
}

void TimerService::setup() {
    this->lastRun = millis() + random(0, interval);
    this->service->setup();
}

void TimerService::loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastRun >= interval) {
        lastRun = currentMillis;
        this->service->loop();
    }
}

void TimerService::setInterval(unsigned long interval) {
    this->interval = interval;
}

