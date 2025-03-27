#ifndef ISERVICE_HPP
#define ISERVICE_HPP

class IService {
    public:
        virtual void setup() = 0;
        virtual void loop() = 0;
};

#endif // ISERVICE_HPP