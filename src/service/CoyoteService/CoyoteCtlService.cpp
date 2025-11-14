#include "CoyoteCtlService.hpp"

CoyoteCtlService::CoyoteCtlService(CoyoteService *coyoteService, MQTTService *mqttService) {
    this->coyoteService = coyoteService;
    this->mqttService = mqttService;
}

void CoyoteCtlService::setup(){
    if (inited) {
        return; // If already initialized, skip setup
    }
    coyoteService->setup();
    mqttService->setup();

    coyoteReportTask = new TimerService(new CoyoteReportService(coyoteService, mqttService), COYOTE_CTL_SERVICE_DEFAULT_INTERVAL);
    coyoteReportTask->setup();

    mqttService->addSubscriber(this);
    inited = true; // Mark as initialized

    std::vector<uint16_t> pointsA = PulseDecoder::parse("Dungeonlab+pulse:0,29,43,1,1/0-1,100-1");
    for (uint16_t point : pointsA) {
        coyoteService->addPoint(0, point);
    }
    if(coyoteService->isDeviceConnected()) {
        coyoteService->setStrengthByChannel(0, 10);
    }
}

void CoyoteCtlService::loop() {
    if (!inited) {
        return; // If not initialized, skip loop
    }
    coyoteService->loop();
    coyoteReportTask->loop();
}


void CoyoteCtlService::callback(const char* topic, JsonDocument* json) {
    if (!topic){
        return;
    }
    if (!json){
        return;
    }
    if (!json->containsKey("coyote")) {
        return;
    }
    JsonObject coyote = json->operator[]("coyote").as<JsonObject>();
    if (coyote.containsKey("strength_a")) {
        uint8_t strengthA = coyote["strength_a"];
        coyoteService->setStrengthByChannel(0, strengthA);
    }
    if (coyote.containsKey("strength_b")) {
        uint8_t strengthB = coyote["strength_b"];
        coyoteService->setStrengthByChannel(1, strengthB);
    }
    if(coyote.containsKey("clear_a") && coyote.operator[]("clear_a").is<bool>() && coyote["clear_a"].as<bool>()) {
        coyoteService->clearPulses(0);
    }    
    if(coyote.containsKey("clear_b") && coyote.operator[]("clear_b").is<bool>() && coyote["clear_b"].as<bool>()) {
        coyoteService->clearPulses(1);
    }
    if (coyote.containsKey("wave_a")) {
        String wave = coyote["wave_a"].as<String>();
        std::vector<uint16_t> points = PulseDecoder::parse(wave);
        for (uint16_t point : points) {
            coyoteService->addPoint(0, point);
        }
    }
    if (coyote.containsKey("wave_b")) {
        String wave = coyote["wave_b"].as<String>();
        std::vector<uint16_t> points = PulseDecoder::parse(wave);
        for (uint16_t point : points) {
            coyoteService->addPoint(1, point);
        }
    }
    this->coyoteReportTask->loop(); // Ensure the report task is updated after processing commands
}