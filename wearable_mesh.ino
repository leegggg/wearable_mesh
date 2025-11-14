#include <Arduino.h>
#include <ArduinoJson.h>
#include <CircularBuffer.hpp>
#include <PicoMQTT.h>
#include <Wire.h>
#include <vector>

#include "src/global.hpp"
#include "src/service/IService.hpp"
#include "src/service/TimerService/TimerService.hpp"
#include "src/service/WatchdogService/WatchdogService.hpp"
#include "src/utils/utils.hpp"

#ifdef CLI_ENABLED
#include "src/service/CliService/CliService.hpp"
#endif // CLI_ENABLED
#ifdef SCREEN_ENABLED
#include "src/service/ScreenService/ScreenService.hpp"
#endif // SCREEN_ENABLED
#ifdef WIFI_ENABLED
#include "src/service/WiFiService/WiFiService.hpp"
#endif // WIFI_ENABLED
#ifdef MQTT_ENABLED
#include "src/service/MQTTService/MQTTService.hpp"
#endif // MQTT_ENABLED
#ifdef ENS160_ENABLED
#include "src/service/Ens160Service/Ens160Service.hpp"
#endif // ENS160_ENABLED
#ifdef AHTX0_ENABLED
#include "src/service/AHTService/AHTService.hpp"
#endif // AHTX0_ENABLED
#ifdef DIRECT_PWM_ENABLED
#include "src/service/DirectPWMService/DirectPWMService.hpp"
#endif // DIRECT_PWM_ENABLED
#ifdef COYOTE_ENABLED
#include "src/service/CoyoteService/CoyoteService.hpp"
#include "src/service/CoyoteService/CoyoteV3ClientService.hpp"
#include "src/service/CoyoteService/CoyoteCtlService.hpp"
#include "src/service/CoyoteService/ICoyoteDeviceService.hpp"
#endif // COYOTE_ENABLED

std::vector<IService *> services;

String hostname = "";
Uci *uci = NULL;
TwoWire *wire = NULL;
Logger *logger = NULL;

void setup() {

    logger = new Logger();
    // init serial
    Serial.begin(115200);
    logger->setSerial(&Serial);
    uci = new Uci(APP_NAME);
    services = std::vector<IService *>();

    // do boot delay
    runBootDelay(uci->getInt(UCI_OS_BOOT_DELAY, DEFAULT_OS_BOOT_DELAY), PIN_ON_BOARD_LED);

    // init i2c
    wire = &Wire;
    wire->setPins(uci->getInt(UCI_I2C_SDA, DEFAULT_I2C_SDA), uci->getInt(UCI_I2C_SCL, DEFAULT_I2C_SCL));

    // init hostname
    WiFi.mode(WIFI_STA);
    String macAddr = WiFi.macAddress();
    macAddr.replace(":", "");
    String defaultHostname = String(APP_NAME_SHORT) + "-" + macAddr;
    defaultHostname.toLowerCase();
    hostname = String(uci->get(UCI_HOSTNAME, defaultHostname));
    WiFi.mode(WIFI_OFF);
    logger->log("Hostname: " + hostname);

    // cli
#ifdef CLI_ENABLED
    Cli *cli = new Cli(&Serial, uci->getBool(UCI_CLI_ECHO, true));
    CliService *cliService = new CliService(cli);
    services.push_back(cliService);
#endif

    // wifi
#ifdef WIFI_ENABLED
    TimerService *wifiService = new TimerService(new WiFiService(), WIFI_SAT_RETRY_DELAY);
    wifiService->setup();
    services.push_back(wifiService);
#else
    // Turn off wifi
    WiFi.mode(WIFI_OFF);
#endif

    // mqtt
#ifdef MQTT_ENABLED
    String mesh = uci->get(UCI_MQTT_MESH, hostname);
    String topic = uci->get(UCI_MQTT_TOPIC, APP_NAME "/" + mesh + "/" + hostname);
    MQTTService *mqttService = new MQTTService(
        uci->get(UCI_MQTT_HOST, DEFAULT_MQTT_HOST),
        uci->get(UCI_MQTT_CLIENT, hostname),
        topic,
        uci->getInt(UCI_MQTT_PORT, DEFAULT_MQTT_PORT),
        uci->get(UCI_MQTT_USERNAME, emptyString),
        uci->get(UCI_MQTT_PASSWORD, emptyString),
        uci->getInt(UCI_MQTT_RECONNECT_INTERVAL, DEFAULT_MQTT_RECONNECT_INTERVAL));
    mqttService->setup();
    services.push_back(mqttService);
    logger->setMqtt(mqttService->getPublisher(), mqttService->getReportTopic());
    services.push_back(new TimerService(new WatchdogService(mqttService, &Serial), WATCHDOG_INTERVAL));
#else
    services.push_back(new TimerService(new WatchdogService(NULL, &Serial), WATCHDOG_INTERVAL));
#endif // MQTT_ENABLED

    // watchdog service

#ifdef SCREEN_ENABLED
    if (uci->getBool(UCI_SCREEN_ENABLED, false)) {
        ScreenService *screenService = new ScreenService(
            wire, uci->getUInt8(UCI_SCREEN_ADDRESS, DEFAULT_SCREEN_ADDRESS),
            uci->getUInt8(UCI_SCREEN_WIDTH, DEFAULT_SCREEN_WIDTH),
            uci->getUInt8(UCI_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT));
        screenService->setup();
        services.push_back(screenService);
    }
#endif // SCREEN_ENABLED

    // ens160
#ifdef ENS160_ENABLED
    if (uci->getBool(UCI_ENS160_ENABLED, false)) {
        services.push_back(new TimerService(new Ens160Service(mqttService), uci->getInt(UCI_ENS160_INTERVAL, DEFAULT_ENS160_INTERVAL)));
    }
#endif // ENS160_ENABLED

    // AHT
#ifdef AHTX0_ENABLED
    if (uci->getBool(UCI_AHTX0_ENABLED, false)) {
        services.push_back(new TimerService(new AHTService(mqttService), uci->getInt(UCI_AHTX0_INTERVAL, DEFAULT_AHTX0_INTERVAL)));
    }
#endif // AHTX0_ENABLED

    // Direct PWM Service
#ifdef DIRECT_PWM_ENABLED
    if (uci->getBool(UCI_DIRECT_PWM_ENABLED, false)) {
        DirectPWMService *directPWMService = new DirectPWMService(mqttService, uci->getUInt64(UCI_DIRECT_PWM_ALLOWED_PINS, 0x0000000000000018));
        services.push_back(directPWMService);
    }
#endif // DIRECT_PWM_ENABLED

#ifdef COYOTE_ENABLED
    if(uci->getBool(UCI_COYOTE_ENABLED, false)) {
        ICoyoteDeviceService *coyoteDeviceService = new CoyoteV3ClientService();
        CoyoteService *coyoteService = new CoyoteService(coyoteDeviceService);
        CoyoteCtlService *coyoteCtlService = new CoyoteCtlService(coyoteService, mqttService);
        coyoteCtlService->setup();
        services.push_back(coyoteCtlService);
    }
#endif // COYOTE_ENABLED

    // setup services
    for (IService *service : services) {
        service->setup();
    }
    logger->log("Setup done. Disable OLED logging.");
    logger->setDisplay(NULL);
    Serial.println(ESP_AT_CONST_READY);
}

void loop() {
    for (IService *service : services) {
        service->loop();
    }
}