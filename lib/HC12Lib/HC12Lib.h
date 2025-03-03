#ifndef HC12LIB_H
#define HC12LIB_H

#include <Arduino.h>
#include <SoftwareSerial.h>

class HC12{
    public:
        HC12(uint8_t rx, uint8_t tx, uint8_t setPin);
        void setSleep(bool sleep);
        void setChannel(uint8_t channel);

    private:
        SoftwareSerial *serial;
        int setPin;
        bool sleep = false;
        bool commandMode = false;

        void setCommandMode(bool mode);
        void sendCommand(String command);
};

#endif
