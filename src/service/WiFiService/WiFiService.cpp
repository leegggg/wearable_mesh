#include "WiFiService.hpp"

WiFiService::WiFiService() {
    this->ssid = emptyString;
    this->password = emptyString;
}

void WiFiService::setup() {
    // check if already connected
    if (WiFi.status() == WL_CONNECTED) {
        logger->log("Already connected to WiFi");
        return;
    }
    // check if wifi is in ap mode
    if (WiFi.getMode() == WIFI_MODE_AP) {
        logger->log("WiFi is in AP mode");
        return;
    }
    // setup sat
    logger->log("Connecting to WiFi");
    WiFi.mode(WIFI_STA);
    for (int satIndex = 0; satIndex < WIFI_MAX_SAT; satIndex++) {
        delay(WIFI_SAT_RETRY_DELAY);
        if (!uci->getBool(UCI_WIFI "." UCI_WIFI_SAT "[" + String(satIndex) + "]." UCI_WIFI_ENABLED, false)) {
            logger->log("SAT disabled. Connecting to next WiFi.");
            continue;
        }
        ssid = uci->get(UCI_WIFI "." UCI_WIFI_SAT "[" + String(satIndex) + "]." UCI_WIFI_SSID, emptyString);
        password = uci->get(UCI_WIFI "." UCI_WIFI_SAT "[" + String(satIndex) + "]." UCI_WIFI_KEY, emptyString);
        if (ssid.isEmpty()) {
            logger->log("No SSID found. Connecting to next WiFi.");
            continue;
        }
        WiFi.begin(ssid.c_str(), password.c_str());
        logger->log("Connecting to WiFi: " + ssid);
        delay(WIFI_SAT_RETRY_DELAY);
        int retryCount = 0;
        while (WiFi.status() != WL_CONNECTED && retryCount < WIFI_SAT_MAX_RETRY) {
            WiFi.reconnect();
            retryCount++;
            delay(WIFI_SAT_RETRY_DELAY);
        }
        if (WiFi.status() == WL_CONNECTED) {
            logger->log("Connected to WiFi: " + ssid + " IP: " + WiFi.localIP().toString());
            break;
        }
        logger->log("Failed to connect to WiFi: " + ssid);
    }
    // setup ap
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.disconnect();
        WiFi.mode(WIFI_MODE_AP);
        String defaultKey = WiFi.macAddress();
        defaultKey.replace(":", "");
        password = uci->get(UCI_WIFI "." UCI_WIFI_AP "." UCI_WIFI_KEY, defaultKey);
        ssid = uci->get(UCI_WIFI "." UCI_WIFI_AP "." UCI_WIFI_SSID, hostname);
        WiFi.softAP(ssid, password);
        logger->log("AP: " + ssid + " Password: " + password + " IP: " + WiFi.softAPIP().toString());
    }
}

void WiFiService::loop() {
    if (WiFi.getMode() == WIFI_MODE_AP) {
        return;
    }
    if (WiFi.status() == WL_CONNECTED) {
        return;
    }
    WiFi.reconnect();
    logger->log("Reconnecting to WiFi");
}