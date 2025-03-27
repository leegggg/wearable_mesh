#ifndef AHT_SERVICE_HPP
#define AHT_SERVICE_HPP

#include "../../global.hpp"
#include "../IService.hpp"
#include "../MQTTService/MQTTService.hpp"
#include <Adafruit_AHTX0.h>

class AHTService : public IService {
    public:
        AHTService(MQTTService* mqttService);
        void setup();
        void loop();
        float getTemperature();
        float getHumidity();
        boolean available();
        static uint32_t calcAbsoluteHumidity(float temperature, float humidity);
        uint32_t getAbsoluteHumidity();
    private:
        Adafruit_AHTX0 *ahtx0;
        float temperature = 0;
        float humidity = 0;
        boolean initialized = false;
        u8_t status = 0xFF;
        MQTTService* mqttService;
};


#endif // AHT_SERVICE_HPP