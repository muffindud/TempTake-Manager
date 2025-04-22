#include "HC12Lib.h"

HC12::HC12(uint8_t rx, uint8_t tx, uint8_t setPin){
    serial = &Serial1;
    this->setPin = setPin;
    pinMode(setPin, OUTPUT);
    serial->begin(1200, SERIAL_8N1, rx, tx);

    // this->setCommandMode(true);
    // this->sendCommand("AT+1200");
    // this->sendCommand("AT+P8");
    // this->sendCommand("AT+FU3");
    // this->setCommandMode(false);
}

void HC12::setCommandMode(bool mode){
    commandMode = mode;
    digitalWrite(this->setPin, mode);
    // this->serial->end();
    // this->serial->begin(mode ? 1200 : 9600);
    delay(100);
}

PACKET_T HC12::receiveData(){
    if(this->serial->available()){
        uint8_t dataStream[PACKET_SIZE];
        this->serial->readBytes(dataStream, PACKET_SIZE);
        PACKET_T packet;
        memcpy(&packet, dataStream, PACKET_SIZE);
        return packet;
    }

    return PACKET_T();
}

void HC12::sendCommand(String command){
    if(this->commandMode){
        this->serial->println(command);
        delay(100);
    }else{
        this->setCommandMode(true);
        this->serial->println(command);
        delay(100);
        this->setCommandMode(false);
    }
}

void HC12::setSleep(bool sleep){
    this->sleep = sleep;
    this->sendCommand(sleep ? "AT+SLEEP" : "AT");
}

void HC12::setChannel(uint8_t channel){
    if(channel < 1 || channel > 127) return;

    this->sendCommand("AT+C" + String(channel));
}
