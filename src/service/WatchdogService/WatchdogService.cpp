#include "WatchdogService.hpp"

WatchdogService::WatchdogService(MQTTService *mqttService, HWCDC* serial) {
    if(mqttService){
        this->mqtt = mqttService->getPublisher();
        this->topic = mqttService->getReportTopic();
    }else{
        this->mqtt = NULL;
    }
    this->serial = serial;
}

void WatchdogService::setup() {
    logger->log("Setting up Watchdog Service");
}

void WatchdogService::loop() {
    JsonDocument doc;
    doc["watchdog"]["ts"] = millis();
    doc["watchdog"]["msg"] = "ok";
    doc["watchdog"]["free"] = String(heap_caps_get_free_size(MALLOC_CAP_8BIT));
    String res;
    serializeJson(doc, res);
    serial->println(res);
    if(mqtt){
        mqtt->publish(topic.c_str(), res.c_str());
    }
}