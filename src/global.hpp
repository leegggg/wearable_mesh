#ifndef WEARABLE_GLOBAL_HPP
#define WEARABLE_GLOBAL_HPP

#include "device/uci/uci.hpp"
#include "device/cli/cli.hpp"
#include "device/logger/logger.hpp"

#define DEBUG
#define SERVICE_QUEUE_SIZE 32

// Config service status
#define CLI_ENABLED
// #define SCREEN_ENABLED
#define WIFI_ENABLED
#define MQTT_ENABLED
// #define AHTX0_ENABLED
// #define ENS160_ENABLED   // disable  this as for sensor too slow
// #define COYOTE_ENABLED
#define DIRECT_PWM_ENABLED


// Config service dependencies
#ifndef WIFI_ENABLED
#undef MQTT_ENABLED
#endif // WIFI_ENABLED

#ifndef MQTT_ENABLED
#undef AHTX0_ENABLED
#undef ENS160_ENABLED
#undef DIRECT_PWM_ENABLED
#endif // MQTT_ENABLED



// Config basic settings
#define APP_NAME "wearable"
#define APP_NAME_SHORT "wab"
#define PIN_ON_BOARD_LED 8
#define DEFAULT_SENSOR_INTERVAL 200

#define UCI_HOSTNAME "os.hostname"
#define UCI_OS_BOOT_DELAY "os.boot_delay"
#define DEFAULT_OS_BOOT_DELAY 3

#define UCI_I2C_SDA "i2c.sda"
#define UCI_I2C_SCL "i2c.scl"
#define DEFAULT_I2C_SDA 8
#define DEFAULT_I2C_SCL 2

#ifdef CLI_ENABLED
#define UCI_CLI_ECHO "cli.echo"
#endif // CLI_ENABLED

#ifdef SCREEN_ENABLED
#define UCI_SCREEN_WIDTH "screen.width"
#define UCI_SCREEN_HEIGHT "screen.height"
#define DEFAULT_SCREEN_WIDTH 128
#define DEFAULT_SCREEN_HEIGHT 64
#define UCI_SCREEN_ADDRESS "screen.address"
#define DEFAULT_SCREEN_ADDRESS 0x3C
#endif // SCREEN_ENABLED

#ifdef WIFI_ENABLED
#define WIFI_MAX_SAT 4
#define WIFI_SAT_MAX_RETRY 5
#define WIFI_SAT_RETRY_DELAY 1000
#define UCI_WIFI "wireless"
#define UCI_WIFI_SAT "sat"
#define UCI_WIFI_AP "ap"
#define UCI_WIFI_ENABLED "enabled"
#define UCI_WIFI_SSID "ssid"
#define UCI_WIFI_KEY "key"
#else
#undef MQTT_ENABLED
#endif // WIFI_ENABLED

#ifdef MQTT_ENABLED
#include <PicoMQTT.h>
#define DEFAULT_MQTT_HOST "broker-cn.emqx.io"
#define DEFAULT_MQTT_PORT 1883
#define DEFAULT_MQTT_RECONNECT_INTERVAL 5000

#define UCI_MQTT_HOST "mqtt.host"
#define UCI_MQTT_CLIENT "mqtt.client"
#define UCI_MQTT_TOPIC "mqtt.topic"
#define UCI_MQTT_MESH "mqtt.mesh"
#define UCI_MQTT_PORT "mqtt.port"
#define UCI_MQTT_USERNAME "mqtt.username"
#define UCI_MQTT_PASSWORD "mqtt.password"
#define UCI_MQTT_RECONNECT_INTERVAL "mqtt.reconnect_interval"
#endif // MQTT_ENABLED

#define WATCHDOG_INTERVAL 10000

#ifdef ENS160_ENABLED
#define UCI_ENS160_INTERVAL "ens160.interval"
#define DEFAULT_ENS160_INTERVAL DEFAULT_SENSOR_INTERVAL
#endif // ENS160_ENABLED

#ifdef AHTX0_ENABLED
#define UCI_AHTX0_INTERVAL "ahtx0.interval"
#define DEFAULT_AHTX0_INTERVAL DEFAULT_SENSOR_INTERVAL
#endif // AHTX0_ENABLED

#ifdef DIRECT_PWM_ENABLED
#define UCI_DIRECT_PWM_ALLOWED_PINS "direct_pwm.allowed_pins"
#endif // DIRECT_PWM_ENABLED

extern String hostname;
extern Uci* uci;
extern TwoWire* wire;
extern Logger* logger;

#endif // WEARABLE_GLOBAL_HPP