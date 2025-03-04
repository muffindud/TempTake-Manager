#include "NetworkLib.h"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
Preferences preferences;

bool initWiFi(){
    // Start preferences with read/write access
    preferences.begin("ssid", false);
    preferences.begin("password", false);

    // Retrieve credentials from preferences
    WIFI_CREDENTIALS_T credentials = getWiFiCredentials();

    #ifdef DEBUG
    Serial.println("Retrieved credentials from preferences...");
    Serial.print("SSID: ");
    Serial.println(credentials.ssid);
    Serial.print("Password: ");
    Serial.println(credentials.password);
    #endif

    // If no credentials are found, listen for them via Bluetooth
    if(strlen(credentials.ssid) == 0){
        #ifdef DEBUG
        Serial.println("No credentials found");
        Serial.println("Listening for credentials");
        #endif

        credentials = listenForCredentials();
    }

    // Connect to WiFi
    if(connectToWiFi(credentials)){
        #ifdef DEBUG
        Serial.println("Connected to WiFi");
        #endif

        // Save credentials to preferences
        saveWiFiCredentials(credentials);

        return true;
    }

    return false;
}

WIFI_CREDENTIALS_T getWiFiCredentials(){
    WIFI_CREDENTIALS_T credentials;

    preferences.getBytes("ssid", credentials.ssid, 32);
    preferences.getBytes("password", credentials.password, 32);
}

WIFI_CREDENTIALS_T listenForCredentials(){
    BluetoothSerial bluetoothSerial;
    WIFI_CREDENTIALS_T credentials;

    String deviceName = "TempTake Manager " + String(ESP.getEfuseMac(), HEX);

    #ifdef DEBUG
    Serial.print("Listening as " + deviceName);
    #endif

    bluetoothSerial.begin(deviceName);

    for(;;){
        if(bluetoothSerial.available()){
            bluetoothSerial.println("Enter credentials in the format: 'ssid:password' or 'ssid' for open networks");
            String message = bluetoothSerial.readString();
            message.trim();

            uint8_t separatorIndex = message.indexOf(':');

            if(separatorIndex == -1){
                strcpy(credentials.ssid, message.c_str());
                strcpy(credentials.password, "");
            } else {
                strcpy(credentials.ssid, message.substring(0, separatorIndex).c_str());
                strcpy(credentials.password, message.substring(separatorIndex + 1).c_str());
            }

            #ifdef DEBUG
            Serial.println("Received credentials:");
            Serial.print("SSID: ");
            Serial.println(credentials.ssid);
            Serial.print("Pass: ");
            Serial.println(credentials.password);
            #endif

            bluetoothSerial.end();
            break;
        }
    }

    return credentials;
}

bool connectToWiFi(WIFI_CREDENTIALS_T credentials){
    if(credentials.ssid == ""){
        return false;
    }else if(credentials.password == ""){
        WiFi.begin(credentials.ssid);
    }else{
        WiFi.begin(credentials.ssid, credentials.password);
    }

    #ifdef DEBUG
    Serial.print("Connecting to WiFi");
    #endif

    for(
        uint8_t attempts = 0;
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

void saveWiFiCredentials(WIFI_CREDENTIALS_T credentials){
    preferences.putBytes("ssid", credentials.ssid, 32);
    preferences.putBytes("password", credentials.password, 32);
}

void deleteWiFiCredentials(){
    preferences.remove("ssid");
    preferences.remove("password");
}
