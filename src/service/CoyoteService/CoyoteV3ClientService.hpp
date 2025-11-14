#ifndef COYOTE_V3_CLIENT_SERVICE_HPP
#define COYOTE_V3_CLIENT_SERVICE_HPP

#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include <NimBLEScan.h>

#include "../IService.hpp"
#include "ICoyoteDeviceService.hpp"
#include "../../global.hpp"

// Coyote V3 BLE Service UUIDs
#define COYOTE_V3_SERVICE_UUID "0000180C-0000-1000-8000-00805f9b34fb"
#define COYOTE_V3_WRITE_CHAR_UUID "0000150A-0000-1000-8000-00805f9b34fb"
#define COYOTE_V3_NOTIFY_CHAR_UUID "0000150B-0000-1000-8000-00805f9b34fb"
#define COYOTE_V3_BATTERY_SERVICE_UUID "0000180A-0000-1000-8000-00805f9b34fb"
#define COYOTE_V3_BATTERY_CHAR_UUID "00001500-0000-1000-8000-00805f9b34fb"

// Coyote V3 Device Name
#define COYOTE_V3_DEVICE_NAME "47L121000"

// Command constants
#define CMD_B0 0xB0  // Main control command
#define CMD_BF 0xBF  // Configuration command
#define CMD_B1 0xB1  // Response message

// Strength parsing methods
#define STRENGTH_NO_CHANGE 0x00
#define STRENGTH_RELATIVE_INCREASE 0x01
#define STRENGTH_RELATIVE_DECREASE 0x02
#define STRENGTH_ABSOLUTE_SET 0x03

class CoyoteV3ClientService : public ICoyoteDeviceService {
    public:
        CoyoteV3ClientService();
        void setup() override;
        void loop() override;
        
        // Public methods for controlling the device
        void setChannelStrength(uint8_t channelA, uint8_t channelB);
        void playWaveform(uint8_t channel, uint32_t intensities, uint32_t frequencies) override;
        uint8_t getChannelStrength(uint8_t channel);
        bool isDeviceConnected() override;
        uint8_t readBatteryLevel();
        
        // Methods required by parent class ICoyoteDeviceService
        uint8_t getBatteryLevel() override;
        void setStrengthByChannel(uint8_t channel, uint8_t strength) override;
        uint8_t getStrengthByChannel(uint8_t channel) override;
        
    private:
        bool inited = false;  // Flag to check if the service is initialized
        // BLE Server components (unused in client mode)
        NimBLEServer *pServer;
        NimBLEService *pService;
        NimBLECharacteristic *pCharacteristic;
        
        // BLE Client components
        NimBLEClient *pClient;
        NimBLERemoteService *pRemoteService;
        NimBLERemoteCharacteristic *pWriteCharacteristic;
        NimBLERemoteCharacteristic *pNotifyCharacteristic;
        NimBLERemoteCharacteristic *pBatteryCharacteristic;
        const NimBLEAdvertisedDevice *targetDevice;
        
        // State management
        bool isConnected;
        uint8_t sequenceNumber;
        uint8_t channelAStrength;
        uint8_t channelBStrength;
        uint8_t batteryLevel;
        unsigned long lastCommandTime;
        
        // Private methods
        bool connectToDevice();
        void sendB0Command();
        void sendBFCommand(uint8_t channelALimit, uint8_t channelBLimit, 
                          uint8_t channelAFreqBalance1, uint8_t channelBFreqBalance1,
                          uint8_t channelAFreqBalance2, uint8_t channelBFreqBalance2);
        void onNotify(NimBLERemoteCharacteristic* pChar, uint8_t* pData, size_t length, bool isNotify);
        void onBatteryNotify(NimBLERemoteCharacteristic* pChar, uint8_t* pData, size_t length, bool isNotify);
        
        // Callback classes
        class CoyoteClientCallbacks : public NimBLEClientCallbacks {
            public:
                CoyoteClientCallbacks(CoyoteV3ClientService* service) : service(service) {}
                void onConnect(NimBLEClient* pClient) override;
                void onDisconnect(NimBLEClient* pClient, int reason) override;
            private:
                CoyoteV3ClientService* service;
        };
        
        class CoyoteScanCallbacks : public NimBLEScanCallbacks {
            public:
                CoyoteScanCallbacks(CoyoteV3ClientService* service) : service(service) {}
                void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override;
            private:
                CoyoteV3ClientService* service;
        };
};

#endif // COYOTE_V3_CLIENT_SERVICE_HPP
