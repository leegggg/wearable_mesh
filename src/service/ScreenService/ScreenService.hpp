#ifndef SCREEN_SERVICE_HPP
#define SCREEN_SERVICE_HPP

#include "../IService.hpp"
#include "../../global.hpp"

class ScreenService : public IService {
    public:
        ScreenService(TwoWire* wire=&Wire, u8_t address=0x3c, u8_t width=128, u8_t height=64);
        void setup();
        void loop();
        Adafruit_SSD1306* getDisplay(){ return display; };
    private:
        TwoWire* wire;
        u8_t address;
        u8_t width;
        u8_t height;
        Adafruit_SSD1306* display;
};

#endif // SCREEN_SERVICE_HPP
