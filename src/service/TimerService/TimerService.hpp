#ifndef TIMER_SERVICE_HPP
#define TIMER_SERVICE_HPP

#include "../IService.hpp"
#include <Arduino.h>

#define TIMER_SERVICE_DEFAULT_INTERVAL 1000

class TimerService : public IService {
    public:
        TimerService(IService* service, unsigned long interval = TIMER_SERVICE_DEFAULT_INTERVAL);
        void setup();
        void loop();
        void setInterval(unsigned long interval);
    private:
        IService* service;
        unsigned long lastRun = 0;
        unsigned long interval = TIMER_SERVICE_DEFAULT_INTERVAL;
};


#endif // TIMER_SERVICE_HPP