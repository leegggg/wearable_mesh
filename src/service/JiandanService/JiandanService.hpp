#ifndef JIANDAN_SERVICE_HPP
#define JIANDAN_SERVICE_HPP

/* 
Service to control devices with Jiandanzhineng MQTT commands.
*/

#include "../IService.hpp"
#include "../MQTTService/MQTTService.hpp"
#include "../../global.hpp"

class JiandanService : public IService {
    public:
        JiandanService(MQTTService* mqttService);
        void setup();
        void loop();
    private:
        // MQTT Service
        MQTTService* mqttService;
        boolean alreadySetup = false;
        void publishCoyoteCommand(JsonDocument* json);
        uint8_t shock = 1;
        uint8_t voltage = 0;
        
};

#endif // JIANDAN_SERVICE_HPP