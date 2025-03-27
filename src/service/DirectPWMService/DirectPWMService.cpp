#include "DirectPWMService.hpp"

DirectPWMService::DirectPWMService(MQTTService *mqttService, uint64_t allowedPinsMask=0) {
    this->allowedPinsMask = allowedPinsMask;
    this->mqttService = mqttService;
}

boolean DirectPWMService::isAllowedPin(uint8_t pin) {
    return (this->allowedPinsMask >> pin) & 1;
}

void DirectPWMService::setAllowedPin(uint8_t pin) {
    this->allowedPinsMask |= (1 << pin);
}

void DirectPWMService::setup() {
    if (this->alreadySetup) {
        return;
    }
    logger->log("DirectPWMService::Setting up");
    this->mqttService->addSubscriber(this);
    String allowedPinsMsg = "DirectPWMService::Setup Allowed pins: ";
    for(int i=0; i <= MAX_PIN; i++) {
        if (isAllowedPin(i)) {
            pinMode(i, OUTPUT);
            allowedPinsMsg += String(i) + ", ";
        }
    }
    logger->log(allowedPinsMsg);
    this->alreadySetup = true;
}

void DirectPWMService::loop() {
}

void DirectPWMService::callback(const char *topic, JsonDocument *json) {
    /*
    {
        "pwm": {
            "pin": 7,
            "value": 128,
        }
    }
    */
    if (!topic){
        return;
    }
    if (!json){
        return;
    }
    if (!json->containsKey("pwm")) {
        return;
    }

    JsonObject pwm = json->operator[]("pwm").as<JsonObject>();
    if (!(pwm.containsKey("pin") && pwm.containsKey("value"))) {
        return;
    }
    uint8_t pin = pwm["pin"];
    uint8_t value = pwm["value"];
    // check if pin is allowed
    if(!isAllowedPin(pin)){
        return;
    }
#ifdef DEBUG
    logger->log("DirectPWMService: pin: " + String(pin) + ", value: " + String(value));
#endif
    analogWrite(pin, value);
}
