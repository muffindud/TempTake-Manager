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

bool initWiFi();
void uploadData(uint8_t* data);
void registerWorker(String workerMac);

#endif