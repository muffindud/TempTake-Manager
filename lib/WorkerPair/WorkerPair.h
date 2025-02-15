#ifndef WORKERPAIR_H
#define WORKERPAIR_H

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <config.h>

#define PAIR_ADDR 0x55
#define PAIR_BUTTON_PIN 19

struct MAC_ADDRESS_T{
    uint8_t mac[6];
};

/*
* @brief Exchange the MAC address with the worker module over the I2C bus
* @param manager_mac: The MAC address of the manager module
*/
void pairWorker(MAC_ADDRESS_T manager_mac, PubSubClient client);

/*
* @brief Save the MAC address of the worker module in the database over MQTT
*/
void addWorkerMac(MAC_ADDRESS_T worker_mac, PubSubClient client);

#endif
