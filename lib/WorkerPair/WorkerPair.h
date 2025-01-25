#ifndef WORKERPAIR_H
#define WORKERPAIR_H

#include <Arduino.h>
#include <Wire.h>

#define PAIR_ADDR 0x55
#define PAIR_BUTTON_PIN 19

struct MAC_ADDRESS_T{
    uint8_t mac[6];
};

/*
* @brief Exchange the MAC address with the worker module over the I2C bus
* @param worker_mac The MAC address of the worker module
*/
void pairWorker(MAC_ADDRESS_T manager_mac);

/*
* @brief Save the MAC address of the worker module in the database over MQTT
*/
void addWorkerMac(MAC_ADDRESS_T worker_mac);

#endif
