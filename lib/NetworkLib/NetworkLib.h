#ifndef NETWORKLIB_H
#define NETWORKLIB_H

#include <Preferences.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <BluetoothSerial.h>

#include "DataPackerLib.h"
#include "WorkerPairLib.h"

#include "config.h"

typedef struct {
    char ssid[32];
    char password[32];
} WIFI_CREDENTIALS_T;

extern WiFiClient wifiClient;
extern PubSubClient mqttClient;
extern Preferences preferences;

/**
 * Connects to WiFi using the stored/retreived credentials
 *
 * @return `true` if connection is successful, `false` otherwise
 */
bool initWiFi();

/**
 * Retrieves WiFi credentials from preferences
 *
 * @param data pointer to the buffer of data
 */
void uploadData(PACKET_T data);

/**
 * Register the worker with the manager
 *
 * @param workerMac the MAC address of the worker
 */
void registerWorker(uint8_t *workerMac);

/**
 * Delete the stored WiFi credentials
 * Should be called if the connection fails
 */
void deleteWiFiCredentials();

#endif