#ifndef I_COYOTE_DEVICE_SERVICE_HPP
#define I_COYOTE_DEVICE_SERVICE_HPP

#include <Arduino.h>
#include "../IService.hpp"

class ICoyoteDeviceService : public IService {
    public:
        // Setup method to initialize the service
        virtual void setup() override = 0;

        // Loop method to handle periodic tasks
        virtual void loop() override = 0;

        // Method to check if the device is connected
        virtual bool isDeviceConnected() = 0;

        virtual uint8_t getBatteryLevel() = 0;

        virtual void setStrengthByChannel(uint8_t channel, uint8_t strength) = 0;
        virtual uint8_t getStrengthByChannel(uint8_t channel) = 0;

        virtual void playWaveform(uint8_t channel, uint32_t intensities, uint32_t frequencies) = 0;

        // Method to read the battery level

};


class ICoyoteDeviceServiceSubscriber {
    public:
        // Callback method to handle incoming messages
        virtual void batteryLevelCallback(uint8_t level) = 0;
        virtual void strengthCallback(uint8_t channel, uint8_t strength) = 0;
};

#endif // I_COYOTE_DEVICE_SERVICE_HPP