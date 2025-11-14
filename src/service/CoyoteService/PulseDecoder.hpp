#ifndef PULSE_DECODER_HPP
#define PULSE_DECODER_HPP
#include <vector>
#include <cstdint>
#include <Arduino.h>

class PulseDecoder {
    public:
        PulseDecoder() = delete; // Prevent instantiation
        static std::vector<uint16_t> parse(String data);
        static uint8_t getFrequencyFromPoint(uint16_t point);
        static uint8_t getStrengthFromPoint(uint16_t point);
    private:
        static uint8_t freqToRaw(int32_t freq);
};

#endif // PULSE_DECODER_HPP