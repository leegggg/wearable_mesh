#include "ScreenService.hpp"

ScreenService::ScreenService(TwoWire* wire, u8_t address, u8_t width, u8_t height) {
    this->wire = wire;
    this->address = address;
    this->width = width;
    this->height = height;
}


void ScreenService::setup() {
    // init oled
    if(display){
        return;
    }
    display = new Adafruit_SSD1306(this->width, this->height, this->wire);
    if(!display->begin(SSD1306_SWITCHCAPVCC, this->address)){
        logger->log("SSD1306 allocation failed");
        return;
    };
    display->display();
    delay(500);
    display->clearDisplay();

    display->fillScreen(SSD1306_WHITE);
    display->display();
    delay(500);
    display->clearDisplay();
    display->display();
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    logger->setDisplay(display);
}

void ScreenService::loop() {
}
