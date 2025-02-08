#ifndef HC12_H
#define HC12_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "DataPacker.h"

class HC12{
    private:
        int rxPin;
        int txPin;
        int setPin;
        bool commandMode = false;
        bool sleep = false;

        void setCommandMode(bool mode);
        void sendCommand(String command);
    public:
        HC12(int rxPin, int txPin, int setPin);

        /*
        * @brief Set the channel of the HC-12 module
        * @brief The channel must be between 1 and 127
        * @param channel: the channel to set the module to
        */
        void setChannel(uint8_t channel);

        /*
        * @brief Receive data from the HC-12 module
        * @param data_stream: the data stream to store the data in
        * @return `true` if data was received, `false` otherwise
        */
        DAT_T receiveData();

        /*
        * @brief Set the sleep state of the HC-12 module
        * @brief On `true`, the module will 22uA and not be able to send/receive data
        * @brief On `false`, the module will consume 16mA and be able to send/receive data
        * @param state: the state to set the module to
        */
        void setSleep(bool state);

        /*
        * @brief Send ACK packet to the worker
        * @param crc16: CRC16 checksum of the data
        * @param manager_mac*: pointer to MAC address of the manager
        * @param worker_mac*: pointer to MAC address of the worker
        * @param id: ID of the packet
        * @param index_packet: Index of the packet
        * @param total_packet_s: Total packets
        */
        void sendAck(
            uint16_t crc16,
            uint8_t* manager_mac,
            uint8_t* worker_mac,
            uint16_t id,
            uint8_t index_packet,
            uint8_t total_packet_s
        );
};

#endif
