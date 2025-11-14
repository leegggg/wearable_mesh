#include "CoyoteV3ClientService.hpp"

CoyoteV3ClientService::CoyoteV3ClientService() {
    this->pServer = nullptr;
    this->pService = nullptr;
    this->pCharacteristic = nullptr;
    this->pClient = nullptr;
    this->pRemoteService = nullptr;
    this->pWriteCharacteristic = nullptr;
    this->pNotifyCharacteristic = nullptr;
    this->pBatteryCharacteristic = nullptr;
    this->isConnected = false;
    this->sequenceNumber = 0;
    this->channelAStrength = 0;
    this->channelBStrength = 0;
    this->batteryLevel = 0;
    this->lastCommandTime = 0;
    this->targetDevice = nullptr;
}

void CoyoteV3ClientService::setup() {
    if(this->inited) {
        Serial.println("Coyote V3 Client Service already initialized");
        return;
    }
    Serial.println("Starting Coyote V3 Client Service...");
    
    // Initialize BLE
    NimBLEDevice::init("CoyoteV3Client");
    
    // Create BLE client
    pClient = NimBLEDevice::createClient();
    pClient->setClientCallbacks(new CoyoteClientCallbacks(this));
    
    // Set connection parameters for better performance
    pClient->setConnectionParams(12, 12, 0, 51);
    pClient->setConnectTimeout(5000);
    
    Serial.println("Coyote V3 Client Service initialized");
    this->inited = true;  // Mark the service as initialized

    while(!isDeviceConnected()){
        if (millis() - lastCommandTime > 2500) {  // Try to reconnect every 5 seconds
            connectToDevice();
            lastCommandTime = millis();
        }
    }
}

void CoyoteV3ClientService::loop() {
    // Handle reconnection if disconnected
    if (!isConnected) {
        if (millis() - lastCommandTime > 2500) {  // Try to reconnect every 5 seconds
            connectToDevice();
            lastCommandTime = millis();
        }
    } else {
        // Send periodic B0 command to maintain connection
        if (millis() - lastCommandTime > 2398) {  // Try to reconnect every 5 seconds
            readBatteryLevel();  // Read battery level periodically
            lastCommandTime = millis();
        }
        // This should be done by upper level logic, not here
        // if (millis() - lastCommandTime > 100) {  // Send every 100ms as per spec
        //     sendB0Command();
        //     lastCommandTime = millis();
        // }
    }
}

bool CoyoteV3ClientService::connectToDevice() {
    Serial.println("Scanning for Coyote V3 device...");
    
    // Start scanning
    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setScanCallbacks(new CoyoteScanCallbacks(this));
    pScan->setInterval(2413);
    pScan->setWindow(2327);
    pScan->setActiveScan(true);
    
    bool scanResult = pScan->start(10, false);
    
    if (targetDevice != nullptr) {
        Serial.println("Found Coyote V3 device, attempting to connect...");
        
        // Set address type to 1 (random static address)
        if (pClient->connect(targetDevice)) {
            Serial.println("Connected to Coyote V3 device");
            
            // Get remote service
            pRemoteService = pClient->getService(COYOTE_V3_SERVICE_UUID);
            if (pRemoteService == nullptr) {
                Serial.println("Failed to find service");
                pClient->disconnect();
                return false;
            }
            
            // Get write characteristic
            pWriteCharacteristic = pRemoteService->getCharacteristic(COYOTE_V3_WRITE_CHAR_UUID);
            if (pWriteCharacteristic == nullptr) {
                Serial.println("Failed to find write characteristic");
                pClient->disconnect();
                return false;
            }
            
            // Get notify characteristic
            pNotifyCharacteristic = pRemoteService->getCharacteristic(COYOTE_V3_NOTIFY_CHAR_UUID);
            if (pNotifyCharacteristic == nullptr) {
                Serial.println("Failed to find notify characteristic");
                pClient->disconnect();
                return false;
            }
            
            // Subscribe to notifications
            if (pNotifyCharacteristic->canNotify()) {
                pNotifyCharacteristic->subscribe(true, 
                    [this](NimBLERemoteCharacteristic* pChar, uint8_t* pData, size_t length, bool isNotify) {
                        this->onNotify(pChar, pData, length, isNotify);
                    });
            }
            
            // Get battery characteristic
            NimBLERemoteService* pBatteryService = pClient->getService(COYOTE_V3_BATTERY_SERVICE_UUID);
            if (pBatteryService != nullptr) {
                pBatteryCharacteristic = pBatteryService->getCharacteristic(COYOTE_V3_BATTERY_CHAR_UUID);
                if (pBatteryCharacteristic != nullptr && pBatteryCharacteristic->canNotify()) {
                    pBatteryCharacteristic->subscribe(true,
                        [this](NimBLERemoteCharacteristic* pChar, uint8_t* pData, size_t length, bool isNotify) {
                            this->onBatteryNotify(pChar, pData, length, isNotify);
                        });
                }
            }
            
            isConnected = true;
            
            // Send initial BF command to set safe limits
            sendBFCommand(100, 100, 128, 128, 128, 128);  // Set moderate limits
            readBatteryLevel();  // Read initial battery level
            
            Serial.println("Coyote V3 device setup complete");
            return true;
        } else {
            Serial.println("Failed to connect to device");
        }
    } else {
        Serial.println("Coyote V3 device not found");
    }
    
    return false;
}

void CoyoteV3ClientService::sendB0Command() {
    if (!isConnected || pWriteCharacteristic == nullptr) {
        return;
    }
    
    // Build B0 command according to protocol
    uint8_t command[20];
    
    // Command header
    command[0] = CMD_B0;
    
    // Sequence number (4 bits) + Strength parsing method (4 bits)
    uint8_t strengthMethod = (STRENGTH_NO_CHANGE << 2) | STRENGTH_NO_CHANGE;  // No change for both channels
    command[1] = (sequenceNumber << 4) | strengthMethod;
    
    // Channel strength values (not used when method is NO_CHANGE)
    command[2] = 0;  // Channel A strength
    command[3] = 0;  // Channel B strength
    
    // Channel A waveform frequencies (4 bytes)
    command[4] = 10;   // 25ms
    command[5] = 10;   // 25ms
    command[6] = 10;   // 25ms
    command[7] = 10;   // 25ms
    
    // Channel A waveform intensities (4 bytes)
    command[8] = 0;    // 25ms
    command[9] = 0;    // 25ms
    command[10] = 0;   // 25ms
    command[11] = 0;   // 25ms
    
    // Channel B waveform frequencies (4 bytes)
    command[12] = 0;   // Invalid to disable channel B
    command[13] = 0;
    command[14] = 0;
    command[15] = 0;
    
    // Channel B waveform intensities (4 bytes) - one value > 100 to disable
    command[16] = 0;
    command[17] = 0;
    command[18] = 0;
    command[19] = 101; // Invalid value to disable channel B
    
    // Send command
    pWriteCharacteristic->writeValue(command, 20, false);
}

void CoyoteV3ClientService::sendBFCommand(uint8_t channelALimit, uint8_t channelBLimit, 
                                         uint8_t channelAFreqBalance1, uint8_t channelBFreqBalance1,
                                         uint8_t channelAFreqBalance2, uint8_t channelBFreqBalance2) {
    if (!isConnected || pWriteCharacteristic == nullptr) {
        return;
    }
    
    uint8_t command[7];
    
    // Command header
    command[0] = CMD_BF;
    
    // Channel strength limits
    command[1] = channelALimit;
    command[2] = channelBLimit;
    
    // Frequency balance parameters 1
    command[3] = channelAFreqBalance1;
    command[4] = channelBFreqBalance1;
    
    // Frequency balance parameters 2
    command[5] = channelAFreqBalance2;
    command[6] = channelBFreqBalance2;
    
    // Send command
    pWriteCharacteristic->writeValue(command, 7, false);
    
    Serial.println("Sent BF configuration command");
}

void CoyoteV3ClientService::setChannelStrength(uint8_t channelA, uint8_t channelB) {
    if (!isConnected || pWriteCharacteristic == nullptr) {
        return;
    }
    
    // Limit strength values to valid range
    channelA = constrain(channelA, 0, 200);
    channelB = constrain(channelB, 0, 200);
    
    uint8_t command[20];
    
    // Command header
    command[0] = CMD_B0;
    
    // Increment sequence number for strength changes
    sequenceNumber = (sequenceNumber + 1) & 0x0F;
    
    // Sequence number (4 bits) + Strength parsing method (4 bits)
    uint8_t strengthMethod = (STRENGTH_ABSOLUTE_SET << 2) | STRENGTH_ABSOLUTE_SET;
    command[1] = (sequenceNumber << 4) | strengthMethod;
    
    // Channel strength values
    command[2] = channelA;
    command[3] = channelB;
    
    // Fill waveform data with safe values
    // Channel A waveform frequencies
    command[4] = 10;
    command[5] = 10;
    command[6] = 10;
    command[7] = 10;
    
    // Channel A waveform intensities
    command[8] = 0;
    command[9] = 0;
    command[10] = 0;
    command[11] = 0;
    
    // Channel B waveform frequencies
    command[12] = 10;
    command[13] = 10;
    command[14] = 10;
    command[15] = 10;
    
    // Channel B waveform intensities
    command[16] = 0;
    command[17] = 0;
    command[18] = 0;
    command[19] = 0;
    
    // Send command
    pWriteCharacteristic->writeValue(command, 20, false);
    
    Serial.printf("Set channel strengths: A=%d, B=%d\n", channelA, channelB);
}

uint8_t CoyoteV3ClientService::readBatteryLevel() {
    if (!isConnected || pBatteryCharacteristic == nullptr) {
        return 0xFF;
    }
    
    // Read battery level
    batteryLevel = pBatteryCharacteristic->readValue().getValue<uint8_t>();
    Serial.printf("Battery level: %d%%\n", batteryLevel);
    return batteryLevel;
}

void CoyoteV3ClientService::playWaveform(uint8_t channel, uint32_t intensities, uint32_t frequencies) {
    if (!isConnected || pWriteCharacteristic == nullptr || channel > 1) {
        return;
    }
    
    uint8_t command[20];
    
    // Command header
    command[0] = CMD_B0;
    
    // Sequence number (4 bits) + Strength parsing method (4 bits) - no strength change
    uint8_t strengthMethod = (STRENGTH_NO_CHANGE << 2) | STRENGTH_NO_CHANGE;
    command[1] = (0 << 4) | strengthMethod;
    
    // Channel strength values (not used)
    command[2] = 0;
    command[3] = 0;


    if (channel == 0) {  // Channel A
        // Channel A waveform data
        for (int i = 0; i < 4; i++) {
            uint8_t freq = (frequencies >> (8 * i)) & 0xFF;
            uint8_t intensity = (intensities >> (8 * i)) & 0xFF;
            command[4 + i] = constrain(freq, 10, 240);
            command[8 + i] = constrain(intensity, 0, 100);
        }
        
        // Disable Channel B with invalid data
        for (int i = 0; i < 4; i++) {
            command[12 + i] = 0;
            command[16 + i] = (i == 3) ? 101 : 0;  // Last intensity > 100 to disable
        }
    } else {  // Channel B
        // Disable Channel A with invalid data
        for (int i = 0; i < 4; i++) {
            command[4 + i] = 0;
            command[8 + i] = (i == 3) ? 101 : 0;  // Last intensity > 100 to disable
        }
        
        // Channel B waveform data
        for (int i = 0; i < 4; i++) {
            uint8_t freq = (frequencies >> (8 * i)) & 0xFF;
            uint8_t intensity = (intensities >> (8 * i)) & 0xFF;
            command[12 + i] = constrain(freq, 10, 240);
            command[16 + i] = constrain(intensity, 0, 100);
        }
    }
    
    // Send command
    pWriteCharacteristic->writeValue(command, 20, false);
    #ifdef DEBUG
    Serial.printf("%02X %02X ", frequencies&0xFF, intensities&0xFF);
    #endif // DEBUG
    
    // channel, frequencies in hex, intensities in hex
    // Serial.printf("Playing waveform on channel %c: Frequencies=0x%08X, Intensities=0x%08X\n", 
    //               channel == 0 ? 'A' : 'B', frequencies, intensities);
}

void CoyoteV3ClientService::onNotify(NimBLERemoteCharacteristic* pChar, uint8_t* pData, size_t length, bool isNotify) {
    if (length < 1) return;
    
    if (pData[0] == CMD_B1 && length >= 4) {
        // B1 response message
        uint8_t responseSequence = pData[1];
        uint8_t channelAStrength = pData[2];
        uint8_t channelBStrength = pData[3];
        
        Serial.printf("B1 Response: Seq=%d, A=%d, B=%d\n", responseSequence, channelAStrength, channelBStrength);
        
        // Update internal state
        this->channelAStrength = channelAStrength;
        this->channelBStrength = channelBStrength;
    }
}

void CoyoteV3ClientService::onBatteryNotify(NimBLERemoteCharacteristic* pChar, uint8_t* pData, size_t length, bool isNotify) {
    if (length >= 1) {
        batteryLevel = pData[0];
        Serial.printf("Battery level: %d%%\n", batteryLevel);
    }
}

uint8_t CoyoteV3ClientService::getChannelStrength(uint8_t channel) {
    return (channel == 0) ? channelAStrength : channelBStrength;
}

bool CoyoteV3ClientService::isDeviceConnected() {
    return isConnected;
}

// Methods required by parent class ICoyoteDeviceService
uint8_t CoyoteV3ClientService::getBatteryLevel() {
    return batteryLevel;
}

void CoyoteV3ClientService::setStrengthByChannel(uint8_t channel, uint8_t strength) {
    if (channel == 0) {
        // Channel A
        setChannelStrength(strength, channelBStrength);
    } else if (channel == 1) {
        // Channel B
        setChannelStrength(channelAStrength, strength);
    }
}

uint8_t CoyoteV3ClientService::getStrengthByChannel(uint8_t channel) {
    if (channel == 0) {
        return channelAStrength;
    } else if (channel == 1) {
        return channelBStrength;
    }
    return 0;
}

// Callback implementations
void CoyoteV3ClientService::CoyoteClientCallbacks::onConnect(NimBLEClient* pClient) {
    Serial.println("Connected to Coyote V3 device");
    pClient->updateConnParams(120, 120, 0, 60);
}

void CoyoteV3ClientService::CoyoteClientCallbacks::onDisconnect(NimBLEClient* pClient, int reason) {
    Serial.println("Disconnected from Coyote V3 device");
    if (service != nullptr) {
        service->isConnected = false;
        service->targetDevice = nullptr;
    }
}

void CoyoteV3ClientService::CoyoteScanCallbacks::onResult(const NimBLEAdvertisedDevice* advertisedDevice) {
    Serial.printf("Found advertised device: %s, Address: %s, Type: %d\n", 
                  advertisedDevice->getName().c_str(), 
                  advertisedDevice->getAddress().toString().c_str(),
                  advertisedDevice->getAddressType());
    if (advertisedDevice->getName() == COYOTE_V3_DEVICE_NAME) {
        Serial.println("Found Coyote V3 device!");
        if (service != nullptr) {
            service->targetDevice = advertisedDevice;
        }
        advertisedDevice->getScan()->stop();
    }
}


