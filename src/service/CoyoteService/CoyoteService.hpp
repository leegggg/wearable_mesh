#ifndef COYOTE_SERVICE_HPP
#define COYOTE_SERVICE_HPP

#include "CircularBuffer.hpp"
#include "ICoyoteDeviceService.hpp"
#include "PulseDecoder.hpp"

#define MAX_PULSE_BUFFER_SIZE 2400
#define PULSE_UPDATE_INTERVAL 100 // Interval in milliseconds to update pulses

class CoyoteService : public IService {
  public:
    CoyoteService(ICoyoteDeviceService *device);

    void setup();
    void loop();
    void addPoint(uint8_t channel, uint16_t point);
    void clearPulses(uint8_t channel);
    uint8_t getBatteryLevel();
    uint8_t getStrengthByChannel(uint8_t channel);
    void setStrengthByChannel(uint8_t channel, uint8_t strength);
    bool isDeviceConnected();
    CircularBuffer<uint16_t, MAX_PULSE_BUFFER_SIZE> *pulseBufferA;
    CircularBuffer<uint16_t, MAX_PULSE_BUFFER_SIZE> *pulseBufferB;
    
  private:
    // Circular buffer for pulse data
    ICoyoteDeviceService *device;
    unsigned long lastRun = 0;
    size_t pointIndexA = 0;
    size_t pointIndexB = 0;
    void updatePulses();
    size_t readChannelPoints4InOne(uint8_t channel, uint32_t intensities, uint32_t frequencies);
    size_t readChannelPoints(uint8_t channel, uint32_t* intensities, uint32_t* frequencies);
    bool setupDone = false;
};

#endif // COYOTE_SERVICE_HPP