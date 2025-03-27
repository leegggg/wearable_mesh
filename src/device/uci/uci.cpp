#include "uci.hpp"

Uci::Uci(String uciNamespace) {
    this->perf = Preferences();
    this->perf.begin(uciNamespace.c_str(), false);
    this->perf = perf;
}

Uci::~Uci() {
    this->perf.end();
}

String Uci::hashKey(String key) {
    // Calc hash with crc64 and take the last 15 chars
    uint64_t hash = crc64((uint8_t *)key.c_str(), key.length());
    // hash to str
    char hashStr[16];
    sprintf(hashStr, "%016llx", hash);
    // take last 15 chars
    // Serial.println("CRC32 Hash: " + String(hashStr) + " Key: " + key);
    return String(hashStr).substring(2);
}

String Uci::get(String key) {
    return this->perf.getString(this->hashKey(key).c_str(), "");
}

String Uci::get(String key, String defaultValue) {
    return this->perf.getString(this->hashKey(key).c_str(), defaultValue.c_str());
}

bool Uci::getBool(String key, bool defaultValue) {
    String valueStr = this->get(key);
    if(valueStr.isEmpty()) {
        return defaultValue;
    }
    if (valueStr.equals("0") || valueStr.equals("false") || valueStr.equals("FALSE")) {
        return false;
    }
    return true;
}

uint8_t Uci::getUInt8(String key, uint8_t defaultValue) {
    String valueStr = this->get(key);
    // try to convert to uint8_t
    uint8_t value = defaultValue;
    if(!valueStr.isEmpty()) {
        value = valueStr.toInt();
    }
    return value;
}

uint64_t Uci::getUInt64(String key, uint64_t defaultValue) {
    String valueStr = this->get(key);
    // try to convert to uint64_t
    uint64_t value = defaultValue;
    if(valueStr.isEmpty()) {
        return value;
    }
    if(valueStr.startsWith("0x")) {
        // remove 0x
        valueStr = valueStr.substring(2);
        value = strtoull(valueStr.c_str(), NULL, 16);
        return value;
    }
    if(valueStr.startsWith("0b")) {
        // remove 0b
        valueStr = valueStr.substring(2);
        value = strtoull(valueStr.c_str(), NULL, 2);
        return value;
    }
    // octave
    if(valueStr.startsWith("0")) {
        value = strtoull(valueStr.c_str(), NULL, 8);
        return value;
    }
    return value;
}

int32_t Uci::getInt(String key, int32_t defaultValue) {
    String valueStr = this->get(key);
    // try to convert to int
    int32_t value = defaultValue;
    if(!valueStr.isEmpty()) {
        // TODO: check if valueStr is a number
        value = valueStr.toInt();
    }
    return value;
}

float Uci::getFloat(String key, float defaultValue) {
    String valueStr = this->get(key);
    // try to convert to float
    float value = defaultValue;
    if(!valueStr.isEmpty()) {
        value = valueStr.toFloat();
    }
    return value;
}

double Uci::getDouble(String key, double defaultValue) {
    String valueStr = this->get(key);
    // try to convert to double
    double value = defaultValue;
    if(!valueStr.isEmpty()) {
        value = valueStr.toDouble();
    }
    return value;
}

bool Uci::set(String key, String value) {
    return this->perf.putString(this->hashKey(key).c_str(), value.c_str());
}

bool Uci::remove(String key) {
    return this->perf.remove(this->hashKey(key).c_str());
}

String Uci::handleATCommand(String command) {
    // Serial.println("Command: " + command);
    if (command.startsWith("AT+UCI=")) {
        // AT+UCI=?
        if (command.equals("AT+UCI=?")) {
            return "AT+UCI=GET,key\nAT+UCI=SET,key,value\nAT+UCI=REMOVE,key";
        }
        // AT+UCI=GET,key
        if (command.startsWith("AT+UCI=GET,")) {
            String key = command.substring(11);
            return this->get(key);
        }
        // AT+UCI=SET,key,value
        else if (command.startsWith("AT+UCI=SET,")) {
            String uciStr = command.substring(11);
            int commaIndex = uciStr.indexOf(',');
            String key = uciStr.substring(0, commaIndex);
            String value = uciStr.substring(commaIndex + 1);
            return this->set(key, value) ? "OK" : "ERROR";
        }
        // AT+UCI=REMOVE,key
        else if (command.startsWith("AT+UCI=REMOVE,")) {
            String key = command.substring(14);
            return this->remove(key) ? "OK" : "ERROR";
        }
        // AT+UCI=RESET
        else if (command.equals("AT+UCI=RESET")) {
            this->perf.clear();
            return "OK";
        }
    }
    return "ERROR";
}