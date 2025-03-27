#ifndef UCI_HPP
#define UCI_HPP

#include <Arduino.h>
#include <Preferences.h>
#include <CRC.h>


class Uci {
public:
    Uci(String uciNamespace);
    ~Uci();
    String get(String key);
    String get(String key, String defaultValue);
    bool getBool(String key, bool defaultValue);
    uint8_t getUInt8(String key, uint8_t defaultValue);
    int32_t getInt(String key, int32_t defaultValue);
    uint64_t getUInt64(String key, uint64_t defaultValue);
    float getFloat(String key, float defaultValue);
    double getDouble(String key, double defaultValue);
    bool set(String key, String value);
    bool remove(String key);
    String handleATCommand(String command);
private:
    Preferences perf;
    String hashKey(String key);
};

#endif // UCI_HPP