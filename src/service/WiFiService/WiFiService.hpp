#ifndef WIFI_SERVICE_HPP
#define WIFI_SERVICE_HPP

#include "../IService.hpp"
#include "../../global.hpp"


class WiFiService : public IService {
    public:
        WiFiService();
        void setup();
        void loop();
        String ssid;
        String password;
};


#endif // WIFI_SERVICE_HPP