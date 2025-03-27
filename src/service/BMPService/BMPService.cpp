#include "BMPService.hpp"

BMPService::BMPService(MQTTService *mqttService) {
    this->mqttService = mqttService;
    this->bmp280 = new Adafruit_BMP280();
}

void BMPService::setup() {
    logger->log("Setting up BMP Service");
    if (bmp280->begin()) {
        logger->log("BMP280 initialized");
    } else {
        logger->log("BMP280 initialization failed");
        this->status = 0xFF;
        return;
    }
    bmp280->setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                        Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                        Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                        Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                        Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void BMPService::loop() {
    if(!available()){
        return;
    }

    this->pressure = bmp280->readPressure();
    this->temperature = bmp280->readTemperature();

    JsonDocument doc;
    doc["bmp280"]["ts"] = millis();
    doc["bmp280"]["temperature"] = this->temperature;
    doc["bmp280"]["pressure"] = this->pressure;

    char json[128];
    serializeJson(doc, json);
    this->mqttService->getPublisher()->publish(this->mqttService->getReportTopic(), json);
}

float BMPService::getTemperature() {
    return this->temperature;
}

float BMPService::getPressure() {
    return this->pressure;
}

boolean BMPService::available() {
    return this->status != 0xFF;
}