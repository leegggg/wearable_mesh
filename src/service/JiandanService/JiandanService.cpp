#include "JiandanService.hpp"
JiandanService::JiandanService(MQTTService* mqttService) {
    this->mqttService = mqttService;
}
void JiandanService::setup() {
    if (this->alreadySetup) {
        return;
    }
    logger->log("JiandanService::Setting up");
    this->alreadySetup = true;

    String mac = WiFi.macAddress();
    mac.replace(":", "");
    mac.toLowerCase();

    String requestTopic = "/drecv/" + mac;
    logger->log("JiandanService::Subscribing to topic: " + requestTopic);

    this->mqttService->getListener()->subscribe(requestTopic.c_str(), [this](const char *topic, const char *payload) {
        logger->log(String("JiandanService::Received message in topic '") + String(topic) + ": " + String(payload));
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        if(error){
            logger->log("Failed to parse JSON");
            return;
        }
        this->publishCoyoteCommand(&doc);
    });
}

void JiandanService::publishCoyoteCommand(JsonDocument* doc) {
    /*
    {
        "method": "set",
        "key": "voltage",
        "value": 50,
        "msg_id": 2001
    }
    {
        "method": "set",
        "key": "shock",
        "value": 1,
        "msg_id": 2002
    }
    */
    if(!doc->containsKey("method")) {
        return;
    }
    if(!doc->containsKey("key")) {
        return;
    }
    if(!doc->containsKey("value")) {
        return;
    }

    if(!String("set").equals(doc->operator[]("method").as<String>())) {
        return;
    }
    if(String("voltage").equals(doc->operator[]("key").as<String>())) {
        this->voltage = doc->operator[]("value");
        if(this->voltage > 100){
            this->voltage = 100;
        }
    }
    if(String("shock").equals(doc->operator[]("key").as<String>())) {
        this->shock = doc->operator[]("value");
        if(this->shock > 1){
            this->shock = 1;
        }
    }
    JsonDocument publishDoc;
    /*
    {
        "coyote": {
            "strength_a": 5,
        }
    }
    */
    JsonObject coyote = publishDoc.createNestedObject("coyote");
    coyote["strength_a"] = this->voltage;
    if(this->shock == 0){
        coyote["strength_a"] = 0;
    }
    logger->log("JiandanService::Publishing coyote command: " + publishDoc.as<String>());
    this->mqttService->getPublisher()->publish(this->mqttService->getRequestTopic().c_str(), publishDoc.as<String>().c_str());
}

void JiandanService::loop() {

}