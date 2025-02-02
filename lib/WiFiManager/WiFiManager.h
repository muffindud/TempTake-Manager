#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <BluetoothSerial.h>
#include <Preferences.h>

#include <config.h>

extern WiFiClient espClient;
extern PubSubClient client;
extern Preferences preferences;

extern String ssid;
extern String pass;

bool initWiFi(String mqtt_server, uint16_t mqtt_port);

bool loadWiFiCredentials();

void listenForCredentials();

void saveWiFiCredentials();

void eraseWiFiCredentials();

#endif
