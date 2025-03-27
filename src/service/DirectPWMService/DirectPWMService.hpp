#ifndef DIRECT_PWM_SERVICE_HPP
#define DIRECT_PWM_SERVICE_HPP

#include "../IService.hpp"
#include "../MQTTService/MQTTService.hpp"
#include "../../global.hpp"

#define MAX_PIN 22

class DirectPWMService : public IService, public MQTTSubscriber {
    public:
        DirectPWMService(MQTTService* mqttService , uint64_t allowedPinsMask);
        void setup();
        void loop();
        void callback(const char* topic, JsonDocument* json);
        boolean isAllowedPin(uint8_t pin);
        void setAllowedPin(uint8_t pin);
    private:
        // Allowed pins
        uint64_t allowedPinsMask = 0;
        // MQTT Service
        MQTTService* mqttService;
        boolean alreadySetup = false;
};

#endif // DIRECT_PWM_SERVICE_HPP