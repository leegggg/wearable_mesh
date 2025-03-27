#ifndef CLI_SERVICE_HPP
#define CLI_SERVICE_HPP

#include "../IService.hpp"
#include "../../global.hpp"

class CliService : public IService {
    public:
        CliService(Cli* cli);
        void setup();
        void loop();
    private:
        Cli* cli;
};

#endif // CLI_SERVICE_HPP