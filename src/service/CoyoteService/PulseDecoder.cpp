#include "PulseDecoder.hpp"
#include <Arduino.h>
#include <cmath>

// Frequency slider value map (from TypeScript)
const int32_t FREQ_SLIDER_VALUE_MAP[] = {
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
    34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 52, 54, 56, 58, 60, 62, 64,
    66, 68, 70, 72, 74, 76, 78, 80, 85, 90, 95, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200,
    233, 266, 300, 333, 366, 400, 450, 500, 550, 600, 700, 800, 900, 1000
};

// Section time map in seconds (from TypeScript)
const float SECTION_TIME_MAP[] = {
    0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f, 1.7f, 1.8f, 1.9f, 2.0f,
    2.1f, 2.2f, 2.3f, 2.4f, 2.5f, 2.6f, 2.7f, 2.8f, 2.9f, 3.0f, 3.1f, 3.2f, 3.3f, 3.4f, 3.5f, 3.6f, 3.7f, 3.8f, 3.9f, 4.0f,
    4.1f, 4.2f, 4.3f, 4.4f, 4.5f, 4.6f, 4.7f, 4.8f, 4.9f, 5.0f, 5.2f, 5.4f, 5.6f, 5.8f, 6.0f, 6.2f, 6.4f, 6.6f, 6.8f, 7.0f, 7.2f,
    7.4f, 7.6f, 7.8f, 8.0f, 8.5f, 9.0f, 9.5f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f, 20.0f, 23.4f, 26.6f, 30.0f, 33.4f,
    36.6f, 40.0f, 45.0f, 50.0f, 55.0f, 60.0f, 70.0f, 80.0f, 90.0f, 100.0f, 120.0f, 140.0f, 160.0f, 180.0f, 200.0f, 250.0f, 300.0f
};

const size_t FREQ_MAP_SIZE = sizeof(FREQ_SLIDER_VALUE_MAP) / sizeof(FREQ_SLIDER_VALUE_MAP[0]);
const size_t TIME_MAP_SIZE = sizeof(SECTION_TIME_MAP) / sizeof(SECTION_TIME_MAP[0]);

uint8_t PulseDecoder::freqToRaw(int32_t freq) {
    if (freq < 10) {
        return 10;
    } else if (freq <= 100) {
        return freq;
    } else if (freq <= 600) {
        return (freq - 100) / 5 + 100;
    } else if (freq <= 1000) {
        return (freq - 600) / 10 + 200;
    } else {
        return 10;
    }
}

std::vector<uint16_t> PulseDecoder::parse(String data) {
    std::vector<uint16_t> points;
    
    if (data.length() == 0) {
        return points;
    }
    
    // Remove prefix "Dungeonlab+pulse:"
    if (data.startsWith("Dungeonlab+pulse:")) {
        data = data.substring(17);
    }
    
    int restDuration = 0;
    
    // Split by "+section+"
    int sectionStart = 0;
    int sectionEnd = data.indexOf("+section+");
    if (sectionEnd == -1) {
        sectionEnd = data.length();
    }
    
    while (sectionStart < data.length()) {
        String section = data.substring(sectionStart, sectionEnd);
        
        // Parse new format: handle "35,1,8=0,20,0,1,1" in first section
        if (sectionStart == 0) {
            int equalPos = section.indexOf('=');
            if (equalPos != -1) {
                String prefix = section.substring(0, equalPos);
                int firstComma = prefix.indexOf(',');
                if (firstComma != -1) {
                    restDuration = prefix.substring(0, firstComma).toInt();
                }
                section = section.substring(equalPos + 1);
            }
        }
        
        // Parse section: "0,32,19,2,1/0-1,16.65-0,33.3-0,50-0,66.65-0,83.3-0,100-1,92-0,84-0,76-0,68-1"
        int slashPos = section.indexOf('/');
        if (slashPos == -1) {
            break;
        }
        
        String header = section.substring(0, slashPos);
        String pulsesStr = section.substring(slashPos + 1);
        
        // Parse header: "0,32,19,2,1"
        int headerParts[5];
        int partIndex = 0;
        int start = 0;
        for (int i = 0; i <= header.length() && partIndex < 5; i++) {
            if (i == header.length() || header.charAt(i) == ',') {
                if (i > start) {
                    headerParts[partIndex++] = header.substring(start, i).toInt();
                }
                start = i + 1;
            }
        }
        
        if (partIndex < 5) {
            break;
        }
        
        int minFreq = headerParts[0];
        int maxFreq = headerParts[1];
        int durationIndex = headerParts[2];
        int mode = headerParts[3];
        int isOn = headerParts[4];
        
        if (durationIndex >= TIME_MAP_SIZE) {
            break;
        }
        
        float duration = SECTION_TIME_MAP[durationIndex];
        
        if (isOn != 0) {
            // Parse pulses: "0-1,16.65-0,33.3-0"
            int pulseCount = 1;
            for (int i = 0; i < pulsesStr.length(); i++) {
                if (pulsesStr.charAt(i) == ',') {
                    pulseCount++;
                }
            }
            
            float pulsesDuration = pulseCount * 10.0f; // Each pulse width is 10 (0.1 seconds)
            int repeatTimes = (int)ceil(duration / (pulsesDuration / 100.0f));
            
            // Parse individual pulses
            String* pulses = new String[pulseCount];
            int pulseIndex = 0;
            int pulseStart = 0;
            for (int i = 0; i <= pulsesStr.length() && pulseIndex < pulseCount; i++) {
                if (i == pulsesStr.length() || pulsesStr.charAt(i) == ',') {
                    if (i > pulseStart) {
                        pulses[pulseIndex++] = pulsesStr.substring(pulseStart, i);
                    }
                    pulseStart = i + 1;
                }
            }
            
            for (int repeat = 0; repeat < repeatTimes; repeat++) {
                for (int index = 0; index < pulseCount; index++) {
                    // Parse pulse: "strength-isOn"
                    int dashPos = pulses[index].indexOf('-');
                    if (dashPos == -1) continue;
                    
                    float strengthFloat = pulses[index].substring(0, dashPos).toFloat();
                    int strength = (int)strengthFloat;
                    
                    int32_t freq = 10;
                    if (mode == 1) {
                        // Constant frequency
                        if (minFreq < FREQ_MAP_SIZE) {
                            freq = FREQ_SLIDER_VALUE_MAP[minFreq];
                        }
                    } else if (mode == 2) {
                        // Frequency increases from minFreq to maxFreq
                        int freqIndex = minFreq + (maxFreq - minFreq) * (pulseCount * repeat + index) / (pulseCount * repeatTimes);
                        if (freqIndex < FREQ_MAP_SIZE) {
                            freq = FREQ_SLIDER_VALUE_MAP[freqIndex];
                        }
                    } else if (mode == 3) {
                        // Frequency changes within each pulse element
                        int freqIndex = minFreq + (maxFreq - minFreq) * index / pulseCount;
                        if (freqIndex < FREQ_MAP_SIZE) {
                            freq = FREQ_SLIDER_VALUE_MAP[freqIndex];
                        }
                    } else if (mode == 4) {
                        // Frequency changes only between pulse elements
                        int freqIndex = minFreq + (maxFreq - minFreq) * repeat / repeatTimes;
                        if (freqIndex < FREQ_MAP_SIZE) {
                            freq = FREQ_SLIDER_VALUE_MAP[freqIndex];
                        }
                    }
                    
                    // Store as [strength, freqRaw] in a single uint16_t
                    // Lower byte: strength, Upper byte: freqRaw
                    uint8_t freqRaw = freqToRaw(freq);
                    points.push_back((uint16_t(freqRaw) << 8) | uint16_t(strength & 0xFF));
                }
            }
            
            // Add rest duration
            for (int i = 0; i < restDuration; i++) {
                points.push_back(0); // [0, 0]
            }
            
            delete[] pulses;
        }
        
        // Move to next section
        sectionStart = sectionEnd + 9; // +9 for "+section+"
        if (sectionStart < data.length()) {
            sectionEnd = data.indexOf("+section+", sectionStart);
            if (sectionEnd == -1) {
                sectionEnd = data.length();
            }
        }
    }
    
    return points;
}

uint8_t PulseDecoder::getFrequencyFromPoint(uint16_t point) {
    return (point >> 8) & 0xFF; // Extract upper byte as frequency
}

uint8_t PulseDecoder::getStrengthFromPoint(uint16_t point) {
    return point & 0xFF; // Extract lower byte as strength
}
