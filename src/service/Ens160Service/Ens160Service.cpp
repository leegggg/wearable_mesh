#include "Ens160Service.hpp"

Ens160Service::Ens160Service(MQTTService *mqttService) {
    this->ens160 = new ScioSense_ENS160(0x53);
    this->ahtx0 = new Adafruit_AHTX0();
    this->mqtt = mqttService->getPublisher();
    this->topic = mqttService->getReportTopic();
}

void Ens160Service::setup() {
    logger->log("Setting up ENS160 Service");
    if(ens160->begin(false)){
        logger->log("ENS160 initialized");
    } else {
        logger->log("ENS160 initialization failed");
    }
    if(ens160->available()){
        logger->log("ENS160 available, Rev" + String(ens160->getMajorRev()) + "." + String(ens160->getMinorRev()) + "." + String(ens160->getBuild()));
    
    }
    if(ahtx0->begin()){
        logger->log("AHTX0 initialized");
    } else {
        logger->log("AHTX0 initialization failed");
    }
    if(ahtx0->getStatus() != 0xFF){
        logger->log("AHTX0 available");
    }
}

void Ens160Service::loop() {
    if(!mqtt){
        return;
    }
    if(!ens160->available()){
        return;
    }

    ens160->measureRaw();
    ens160->measure();

    JsonDocument doc;
    doc["ens160"]["ts"] = millis();
    doc["ens160"]["aqi"] = ens160->getAQI();
    doc["ens160"]["aqi500"] = ens160->getAQI500();
    doc["ens160"]["co2"] = ens160->geteCO2();
    doc["ens160"]["tvoc"] = ens160->getTVOC();
    doc["ens160"]["hp0"] = ens160->getHP0();
    doc["ens160"]["hp0bl"] = ens160->getHP0BL();
    doc["ens160"]["hp1"] = ens160->getHP1();
    doc["ens160"]["hp1bl"] = ens160->getHP1BL();
    doc["ens160"]["hp2"] = ens160->getHP2();
    doc["ens160"]["hp2bl"] = ens160->getHP2BL();
    doc["ens160"]["hp3"] = ens160->getHP3();
    doc["ens160"]["hp3bl"] = ens160->getHP3BL();
    u8_t status = ens160->getMISR();
    char statusStr[8];
    sprintf(statusStr, "0x%02x", status);
    doc["ens160"]["status"] = String(statusStr);

    if(ahtx0->getStatus()!=0xff){
        sensors_event_t temperaturEevent;
        sensors_event_t humidityEvent;
        ahtx0->getEvent(&humidityEvent, &temperaturEevent);
        doc["ahtx0"]["temp"] = temperaturEevent.temperature;
        doc["ahtx0"]["humidity"] = humidityEvent.relative_humidity;
        ens160->set_envdata(temperaturEevent.temperature, humidityEvent.relative_humidity);
    }


    String res;
    serializeJson(doc, res);
    mqtt->publish(topic.c_str(), res.c_str());

}