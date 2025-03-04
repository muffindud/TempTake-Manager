#ifndef NETWORKLIB_H
#define NETWORKLIB_H

#include <Preferences.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <BluetoothSerial.h>

#include "config.h"

typedef struct {
    char ssid[32];
    char password[32];
} WIFI_CREDENTIALS_T;

extern WiFiClient wifiClient;
extern PubSubClient mqttClient;
extern Preferences preferences;

/*
* @brief Connects to WiFi using the stored/retreived credentials
* @return `true` if connection is successful, `false` otherwise
*/
bool initWiFi();

/*
* @brief Retrieves WiFi credentials from preferences
* @param `*data`: pointer to the buffer of data
*/
void uploadData(uint8_t* data);

/*
* @brief Register the worker with the manager
* @param `workerMac`: the MAC address of the worker
*/
void registerWorker(String workerMac);

#endif