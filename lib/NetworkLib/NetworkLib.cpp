#include "NetworkLib.h"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
Preferences preferences;

bool initWiFi(){
    preferences.begin("ssid", false);
    preferences.begin("password", false);

    WIFI_CREDENTIALS_T credentials = getWiFiCredentials();

    #ifdef DEBUG
    Serial.println("Retrieved credentials:");
    Serial.print("SSID: ");
    Serial.println(credentials.ssid);
    Serial.print("Password: ");
    Serial.println(credentials.password);
    #endif

    if(strlen(credentials.ssid) == 0){
        #ifdef DEBUG
        Serial.println("No credentials found");
        Serial.println("Listening for credentials");
        #endif

        credentials = listenForCredentials();
    }

    WiFi.begin(credentials.ssid, credentials.password);
    #ifdef DEBUG
    Serial.print("Connecting to WiFi");
    #endif

    for(int attempts = 0;
        attempts < 10 && WiFi.status() != WL_CONNECTED;
        attempts++
    ){
        #ifdef DEBUG
        Serial.print(".");
        #endif

        delay(1000);
    }

    #ifdef DEBUG
    Serial.println();
    #endif

    if(WiFi.status() == WL_CONNECTED){
        #ifdef DEBUG
        Serial.println("Connected to WiFi");
        #endif

        return true;
    }

    #ifdef DEBUG
    Serial.println("Failed to connect to WiFi");
    #endif

    return false;
}

WIFI_CREDENTIALS_T getWiFiCredentials(){
    WIFI_CREDENTIALS_T credentials;

    preferences.getBytes("ssid", credentials.ssid, 32);
    preferences.getBytes("password", credentials.password, 32);
}

WIFI_CREDENTIALS_T listenForCredentials(){
}

void saveWiFiCredentials(){
}

void deleteWiFiCredentials(){
}
