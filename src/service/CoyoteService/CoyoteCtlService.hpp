#ifndef COYOTE_CTL_SERVICE_HPP
#define COYOTE_CTL_SERVICE_HPP

#include "../IService.hpp"
#include "CoyoteService.hpp"
#include "../MQTTService/MQTTService.hpp"
#include "../TimerService/TimerService.hpp"
#include "PulseDecoder.hpp"

#define COYOTE_CTL_SERVICE_DEFAULT_INTERVAL 5000 // Default interval for CoyoteCtlService in milliseconds

class CoyoteCtlService : public IService, MQTTSubscriber{
    public:
        CoyoteCtlService(CoyoteService *coyoteService, MQTTService *mqttService);
        void setup();
        void loop();
    private:
        bool inited = false;
        CoyoteService *coyoteService;
        MQTTService *mqttService;
        TimerService *coyoteReportTask;
        void callback(const char* topic, JsonDocument* json);
};

class CoyoteReportService : public IService {
    public:
        CoyoteReportService(CoyoteService *coyoteService, MQTTService *mqttService);
        void setup();
        void loop();
    private:
        bool inited = false;
        CoyoteService *coyoteService;
        CoyoteReportService *coyoteReportService;
        MQTTService *mqttService;
};

#endif // COYOTE_CTL_SERVICE_HPP