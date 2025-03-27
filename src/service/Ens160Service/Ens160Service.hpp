#ifndef ENS160_SERVICE_HPP
#define ENS160_SERVICE_HPP

#include "../IService.hpp"
#include "../../global.hpp"
#include "../MQTTService/MQTTService.hpp"
#include <ScioSense_ENS160.h>
#include <Adafruit_AHTX0.h>

class Ens160Service : public IService {
    public:
        Ens160Service(MQTTService *mqttService);
        void setup();
        void loop();
    private:
        ScioSense_ENS160* ens160 = NULL;
        Adafruit_AHTX0* ahtx0 = NULL;
        PicoMQTT::Publisher *mqtt = NULL;
        String topic;
};


#endif // ENS160_SERVICE_HPP