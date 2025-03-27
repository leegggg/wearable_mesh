#include "AHTService.hpp"

AHTService::AHTService(MQTTService *mqttService) {
    this->mqttService = mqttService;
    this->ahtx0 = new Adafruit_AHTX0();
}

void AHTService::setup() {
    logger->log("Setting up AHT Service");
    if(ahtx0->begin()){
        logger->log("AHTX0 initialized");
    } else {
        logger->log("AHTX0 initialization failed");
    }
    if(ahtx0->getStatus() != 0xFF){
        logger->log("AHTX0 available");
        this->status = ahtx0->getStatus();
    }
}

void AHTService::loop() {
    this->status = ahtx0->getStatus();
    if(!available()){
        return;
    }

    sensors_event_t humidityEnv, tempEnv;
    ahtx0->getEvent(&humidityEnv, &tempEnv);
    this->temperature = tempEnv.temperature;
    this->humidity = humidityEnv.relative_humidity;

    JsonDocument doc;
    doc["aht"]["ts"] = millis();
    doc["aht"]["temperature"] = this->temperature;
    doc["aht"]["humidity"] = this->humidity;
    doc["aht"]["absolute_humidity"] = getAbsoluteHumidity();

    char json[128];
    serializeJson(doc, json);
    this->mqttService->getPublisher()->publish(this->mqttService->getReportTopic(), json);
}

float AHTService::getTemperature() {
    return this->temperature;
}

float AHTService::getHumidity() {
    return this->humidity;
}

boolean AHTService::available() {
    return this->status != 0xFF;
}

uint32_t AHTService::calcAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity);                                                                // [mg/m^3]
    return absoluteHumidityScaled;
}

uint32_t AHTService::getAbsoluteHumidity() {
    return calcAbsoluteHumidity(this->temperature, this->humidity);
}

