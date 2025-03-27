#ifndef MQTT_SERVICE_HPP
#define MQTT_SERVICE_HPP


#include <ArduinoJson.h>
#include "../IService.hpp"
#include "../../global.hpp"

#define MQTT_SERVICE_MODE_NULL 0
#define MQTT_SERVICE_MODE_CLIENT 1
#define MQTT_SERVICE_MODE_SERVER 2

class MQTTSubscriber {
    public:
        virtual void callback(const char* topic, JsonDocument* json) = 0;
};

class MQTTService: public IService{
    public:
        MQTTService(String host, String client, String topic, uint16_t port=1883, String username=emptyString, String password=emptyString, unsigned long reconnect_interval_millis=3000);
        void setup();
        void loop();
        String getReportTopic();
        PicoMQTT::SubscribedMessageListener* getListener(){  return mqttListener; }
        PicoMQTT::PicoMQTTInterface* getInterface(){ return mqttInterface; }
        PicoMQTT::Publisher* getPublisher(){ return mqttPublisher; }
        void addSubscriber(MQTTSubscriber* subscriber);
    private:
        void setSubscribe();
        String reportTopic;
        String requestTopic;
        String host;
        int port;
        String client;
        String username;
        String password;
        unsigned long reconnect_interval_millis;
        uint8_t mode = MQTT_SERVICE_MODE_NULL;
        PicoMQTT::SubscribedMessageListener* mqttListener;
        PicoMQTT::PicoMQTTInterface* mqttInterface;
        PicoMQTT::Publisher* mqttPublisher;
        boolean alreadySetup = false;
        std::vector<MQTTSubscriber*> subscribers;
};

#endif // MQTT_SERVICE_HPP