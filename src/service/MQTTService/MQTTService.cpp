#include "MQTTService.hpp"

MQTTService::MQTTService(String host, String client, String topic, uint16_t port, String username, String password, unsigned long reconnect_interval_millis) {
    this->host = host;
    this->port = port;
    this->client = client;
    this->username = username;
    this->password = password;
    this->reconnect_interval_millis = reconnect_interval_millis;
    this->requestTopic = topic + "/request";
    this->reportTopic = topic + "/report";
}

void MQTTService::setup() {
    if(this->alreadySetup){
        return;
    } 
    this->subscribers = std::vector<MQTTSubscriber*>();
    logger->log("Setting up MQTT Service ");
    if(host.isEmpty() || WiFi.getMode() == WIFI_AP){
        mode = MQTT_SERVICE_MODE_SERVER;
        // Setup MQTT broker server
        PicoMQTT::Server* server = new PicoMQTT::Server();
        server->begin();
        logger->log("Begin MQTT Server");
        this->mqttInterface = server;
        this->mqttListener = server;
        this->mqttPublisher = server;
        // TODO: setup auth
    }else{
        mode = MQTT_SERVICE_MODE_CLIENT;
        // Setup MQTT client
        PicoMQTT::Client* mqttClient = new PicoMQTT::Client(host.c_str(), port, client.c_str(), username.c_str(), password.c_str(), reconnect_interval_millis);
        mqttClient->begin();
        logger->log(String("Begin MQTT ") + "host:" + host + ", port:" + port + ", client:" + client + ", username:" + username + ", password:" + password + ", reconnect_interval_millis:" + reconnect_interval_millis);
        logger->log("MQTT request_topic: " + requestTopic + ", report_topic: " + reportTopic);
        this->mqttInterface = mqttClient;
        this->mqttListener = mqttClient;
        this->mqttPublisher = mqttClient;
        // TODO: Enable TLS
    }
    setSubscribe();
    this->alreadySetup = true;
}

String MQTTService::getReportTopic(){
    return this->reportTopic;
}

String MQTTService::getRequestTopic(){
    return this->requestTopic;
}

void MQTTService::setSubscribe(){
    this->mqttListener->subscribe(this->requestTopic.c_str(), [this](const char *topic, const char *payload) {
#ifdef DEBUG
        logger->log(String("Received message in topic '") + String(topic) + ": " + String(payload));
#endif // DEBUG
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        if(error){
            logger->log("Failed to parse JSON");
            return;
        }
        for(MQTTSubscriber* subscriber: subscribers){
            subscriber->callback(topic, &doc);
        }
    });
}

void MQTTService::addSubscriber(MQTTSubscriber* subscriber){
    this->subscribers.push_back(subscriber);
}

void MQTTService::loop() {
    this->mqttInterface->loop();
}