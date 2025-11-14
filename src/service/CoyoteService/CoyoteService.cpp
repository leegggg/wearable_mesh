#include "CoyoteService.hpp"

CoyoteService::CoyoteService(ICoyoteDeviceService *device) {
    this->pulseBufferA = new CircularBuffer<uint16_t, MAX_PULSE_BUFFER_SIZE>();
    this->pulseBufferB = new CircularBuffer<uint16_t, MAX_PULSE_BUFFER_SIZE>();
    this->device = device;
}

void CoyoteService::setup() {
    if(this->setupDone) {
        return; // If setup is already done, skip the setup
    }
    Serial.println("Coyote Service setup called");
    // Initialize the pulse buffer
    this->device->setup();
    this->pulseBufferA->clear();
    this->pulseBufferB->clear();
    this->lastRun = millis();
    this->setupDone = true; // Mark the setup as done
}

void CoyoteService::loop() {
    device->loop(); // Call the device's loop method to handle any device-specific logic
    updatePulses();
}

void CoyoteService::updatePulses() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastRun < PULSE_UPDATE_INTERVAL) {
        return; // Exit if not enough time has passed since the last update
    }
    if (!device->isDeviceConnected()) {
        return; // Exit if device is not connected
    }

    lastRun = currentMillis;

    if (!pulseBufferA->isEmpty()) {
        uint32_t intensities = 0;
        uint32_t frequencies = 0;
        pointIndexA = readChannelPoints(0, &intensities, &frequencies);
        device->playWaveform(0, intensities, frequencies);
    }

    if (!pulseBufferB->isEmpty()) {
        uint32_t intensities = 0;
        uint32_t frequencies = 0;
        pointIndexB = readChannelPoints(1, &intensities, &frequencies);
        device->playWaveform(1, intensities, frequencies);
    }
}

void CoyoteService::addPoint(uint8_t channel, uint16_t point) {
    if (channel == 0) {
        pulseBufferA->push(point);

    } else if (channel == 1) {
        pulseBufferB->push(point);
    }
}

void CoyoteService::clearPulses(uint8_t channel) {
    if (channel == 0) {
        pulseBufferA->clear();
        pointIndexA = 0; // Reset index for channel A
    } else if (channel == 1) {
        pulseBufferB->clear();
        pointIndexB = 0; // Reset index for channel B
    }
}

uint8_t CoyoteService::getBatteryLevel() {
    return device->getBatteryLevel();
}

uint8_t CoyoteService::getStrengthByChannel(uint8_t channel) {
    return device->getStrengthByChannel(channel);
}

void CoyoteService::setStrengthByChannel(uint8_t channel, uint8_t strength) {
    device->setStrengthByChannel(channel, strength);
}

bool CoyoteService::isDeviceConnected() {
    return device->isDeviceConnected();
}

size_t CoyoteService::readChannelPoints4InOne(uint8_t channel, uint32_t intensities, uint32_t frequencies) {
    CircularBuffer<uint16_t, MAX_PULSE_BUFFER_SIZE> *buffer = (channel == 0) ? pulseBufferA : pulseBufferB;
    size_t current_index = (channel == 0) ? pointIndexA : pointIndexB;

    if (buffer->isEmpty()) {
        return 0; // No points to read
    }
    if (current_index + 4 > buffer->size()) {
        current_index = 0; // Reset index if it exceeds buffer size
    }

    intensities = 0;
    frequencies = 0;

    for (size_t i = 0; i < 4; i++) {
        uint16_t point = (*buffer)[current_index++];
        uint8_t freqRaw = (point >> 8) & 0xFF;
        uint8_t strength = point & 0xFF;
        frequencies |= (static_cast<uint32_t>(freqRaw) << (8 * i));
        intensities |= (static_cast<uint32_t>(strength) << (8 * i));
    }

    return current_index + 4;
}

size_t CoyoteService::readChannelPoints(uint8_t channel, uint32_t* intensities, uint32_t* frequencies) {
    CircularBuffer<uint16_t, MAX_PULSE_BUFFER_SIZE> *buffer = (channel == 0) ? pulseBufferA : pulseBufferB;
    size_t current_index = (channel == 0) ? pointIndexA : pointIndexB;

    if (buffer->isEmpty()) {
        return 0; // No points to read
    }

    // Serial.printf("Reading channel %d points from index %zu, point=0x%08x\n", channel, current_index, (*buffer)[current_index]);

    *intensities = 0;
    *frequencies = 0;
    uint16_t point = (*buffer)[current_index];
    uint8_t freqRaw = (point >> 8) & 0xFF;
    uint8_t strength = point & 0xFF;
    for (int i = 0; i < 4; i++) {
        (*frequencies) |= (static_cast<uint32_t>(freqRaw) << (8 * i));
        (*intensities) |= (static_cast<uint32_t>(strength) << (8 * i));
    }

    if (current_index + 1 > buffer->size()) {
        return 0; // Reset index if it exceeds buffer size
    }
    return current_index + 1;
}
