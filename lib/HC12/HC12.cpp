#include "HC12.h"


HC12::HC12(int rxPin, int txPin, int setPin){
    this->rxPin = rxPin;
    this->txPin = txPin;
    this->setPin = setPin;

    // Serial1.begin(1200);
    Serial1.begin(1200, SERIAL_8N1, rxPin, txPin);
    Serial.begin(9600);

    // this->setCommandMode(true);
    // this->sendCommand("AT+B1200");
    // this->sendCommand("AT+P8");
    // this->sendCommand("AT+FU3");
    // this->sendCommand("AT+U8N1");
    // this->setCommandMode(false);

    // Serial.end();
    // serial->end();
    // Serial.begin(1200);
    // serial->begin(1200);
}

void HC12::setCommandMode(bool mode){
    digitalWrite(this->setPin, mode ? HIGH : LOW);
    this->commandMode = mode;
    delay(100);
}

void HC12::sendCommand(String command){
    if (this->commandMode){
        Serial1.println(command);
        delay(100);
    }else{
        this->setCommandMode(true);
        Serial1.println(command);
        this->setCommandMode(false);
    }
}

void HC12::setChannel(uint8_t channel){
    if(channel < 1 || channel > 127){
        return;
    }

    this->sendCommand("AT+C" + String(channel));
}

DAT_T HC12::receiveData(){
    if(Serial1.available()){
        uint8_t* data_stream = (uint8_t*)malloc(DAT_SIZE);
        Serial1.readBytes(data_stream, DAT_SIZE);
        DAT_T data_packet;
        memcpy(&data_packet, data_stream, DAT_SIZE);
        free(data_stream);
        return data_packet;
    }

    return DAT_T();
}

void HC12::sendAck(uint16_t crc16, uint8_t* manager_mac, uint8_t* worker_mac, uint16_t id, uint8_t index_packet, uint8_t total_packet_s){
    ACK_T ack;
    ack.meta.crc16 = crc16;
    memcpy(ack.meta.manager_mac, manager_mac, 6);
    memcpy(ack.meta.worker_mac, worker_mac, 6);
    ack.meta.id = id;
    ack.meta.index_packet = index_packet;
    ack.meta.total_packet_s = total_packet_s;

    Serial1.write((uint8_t*)&ack, ACK_SIZE);
}

void HC12::setSleep(bool state){
    this->sleep = state;
    this->sendCommand(state ? "AT+SLEEP" : "AT");
}
