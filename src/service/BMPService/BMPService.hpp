#ifndef BMP_SERVICE_HPP
#define BMP_SERVICE_HPP

#include "../../global.hpp"
#include "../IService.hpp"
#include "../MQTTService/MQTTService.hpp"
#include <Adafruit_BMP280.h>

class BMPService : public IService {
    public:
        BMPService(MQTTService* mqttService);
        void setup();
        void loop();
        float getTemperature();
        float getPressure();
        boolean available();
    private:
        Adafruit_BMP280 *bmp280;
        float temperature = 0;
        float pressure = 0;
        boolean initialized = false;
        u8_t status = 0xFF;
        MQTTService* mqttService;
};

#endif // BMP_SERVICE_HPP
