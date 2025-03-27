#include "logger.hpp"

Logger::Logger() {
    this->serial = NULL;
    this->mqtt = NULL;
    this->display = NULL;
}

Logger::Logger(Stream *serial, PicoMQTT::Server *mqtt, String topic, Adafruit_SSD1306* display) {
    this->serial = serial;
    this->mqtt = mqtt;
    this->topic = topic;
}

void Logger::setSerial(Stream *serial) { this->serial = serial; }

void Logger::setDisplay(Adafruit_SSD1306* display) { this->display = display; }

void Logger::setMqtt(PicoMQTT::Publisher *mqtt, String topic) {
    this->mqtt = mqtt;
    this->topic = topic;
}

String Logger::get() { 
    this->current_index = this->current_index % LOG_BUFFER_SIZE; 
    return this->log_buffer[this->current_index]; 
}

String Logger::next() {
    this->current_index++;
    return get();
}

String Logger::last() {
    this->current_index--;
    return get();
}

String Logger::tail() {
    this->current_index = this->log_buffer.size() - 1;
    return get();
}

u8_t Logger::currentIndex() { return this->current_index % LOG_BUFFER_SIZE; }

String Logger::get(int index) {
    this->current_index = index % LOG_BUFFER_SIZE;
    return get();
}

void Logger::log(String msg) {
    this->log_buffer.push(String(millis()) + ": \n" + msg);
    JsonDocument doc;
    doc["ts"] = millis();
    doc["msg"] = msg;
    String res;
    serializeJson(doc, res);

    if (this->serial) {
        this->serial->println(res);
    }

    if (this->mqtt) {
        this->mqtt->publish(this->topic, res);
    }

    if(this->display) {
        this->display->clearDisplay();
        this->display->setCursor(0, 0);
        this->display->println(res);
        this->display->display();
    }
}