#ifndef COYOTE_V3_SERVER_SERVICE_HPP
#define COYOTE_V3_SERVER_SERVICE_HPP

#include <NimBLEDevice.h>

#include "../IService.hpp"
#include "../../global.hpp"

class CoyoteV3ServerService : public IService {
    public:
        CoyoteV3ServerService();
        void setup();
        void loop();
    private:
        NimBLEServer *pServer;
        NimBLEService *pService;
        NimBLECharacteristic *pCharacteristic;
};

#endif // COYOTE_V3_SERVER_SERVICE_HPP