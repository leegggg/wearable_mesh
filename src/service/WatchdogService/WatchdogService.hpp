#ifndef WATCHDOG_SERVICE_HPP
#define WATCHDOG_SERVICE_HPP

#include <ArduinoJson.h>
#include "../IService.hpp"
#include "../TimerService/TimerService.hpp"
#include "../../global.hpp"
#include "../../service/MQTTService/MQTTService.hpp"

class WatchdogService: public IService{
    public:
        WatchdogService(MQTTService *mqttService, HWCDC* serial);
        void setup();
        void loop();
    private:
        PicoMQTT::Publisher *mqtt;
        String topic;
        HWCDC* serial;
};

#endif // WATCHDOG_SERVICE_HPP