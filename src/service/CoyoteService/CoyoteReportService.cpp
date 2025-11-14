#include "CoyoteCtlService.hpp"


CoyoteReportService::CoyoteReportService(CoyoteService *coyoteService, MQTTService *mqttService) {
    this->coyoteService = coyoteService;
    this->mqttService = mqttService;
    inited = true;
}

void CoyoteReportService::setup() {
    if (inited) {
        return; // If already initialized, skip setup
    }
    inited = true; // Mark as initialized
}

void CoyoteReportService::loop(){
    if(!coyoteService->isDeviceConnected()){
        return; // Exit if device is not connected
    }
    uint8_t batteryLevel = coyoteService->getBatteryLevel();
    uint8_t strengthA = coyoteService->getStrengthByChannel(0);
    uint8_t strengthB = coyoteService->getStrengthByChannel(1);
    JsonDocument doc;
    doc["coyote"]["battery"] = batteryLevel;
    doc["coyote"]["strength_a"] = strengthA;
    doc["coyote"]["wave_count_a"] = coyoteService->pulseBufferA->size();
    doc["coyote"]["strength_b"] = strengthB;
    doc["coyote"]["wave_count_b"] = coyoteService->pulseBufferB->size();
    doc["coyote"]["connected"] = coyoteService->isDeviceConnected();
    doc["coyote"]["timestamp"] = millis();
    String jsonstr = String();
    serializeJson(doc,jsonstr);
    this->mqttService->getPublisher()->publish(this->mqttService->getReportTopic(), jsonstr);

}
