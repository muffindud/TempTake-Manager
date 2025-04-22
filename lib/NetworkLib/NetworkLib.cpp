#include "NetworkLib.h"

#define PREFERENCES_NAMESPACE "wifi"
#define CREDENTIALS_KEY "WIFI"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
Preferences preferences;
String deviceName;
String managerMac;

WIFI_CREDENTIALS_T getWiFiCredentials();
void registerManager();
void reconnect();
void deleteWiFiCredentials();
void saveWiFiCredentials(WIFI_CREDENTIALS_T credentials);
bool connectToWiFi(WIFI_CREDENTIALS_T credentials);
WIFI_CREDENTIALS_T listenForCredentials();

bool initWiFi(){
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

        // Set MQTT server and port
        mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

        // Register manager with MQTT server
        registerManager();

        #ifdef DEBUG
        Serial.println("MQTT server set: " + String(MQTT_SERVER) + ":" + String(MQTT_PORT));
        #endif

        return true;
    }

    return false;
}

WIFI_CREDENTIALS_T getWiFiCredentials(){
    WIFI_CREDENTIALS_T credentials;

    preferences.begin(PREFERENCES_NAMESPACE, true);
    preferences.getBytes(CREDENTIALS_KEY, &credentials, sizeof(WIFI_CREDENTIALS_T));
    preferences.end();

    return credentials;
}

WIFI_CREDENTIALS_T listenForCredentials(){
    BluetoothSerial bluetoothSerial;
    WIFI_CREDENTIALS_T credentials;

    deviceName = "TempTake Manager " + String(ESP.getEfuseMac(), HEX);

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
        Serial.println("Local IP: " + WiFi.localIP().toString());
        Serial.println("MAC: " + WiFi.macAddress());
        #endif

        uint8_t mac[6];
        WiFi.macAddress(mac);
        managerMac = String(mac[0], HEX) + String(mac[1], HEX) + String(mac[2], HEX) + String(mac[3], HEX) + String(mac[4], HEX) + String(mac[5], HEX);
        managerMac.toUpperCase();

        return true;
    }

    #ifdef DEBUG
    Serial.println("Failed to connect to WiFi");
    #endif

    return false;
}

void saveWiFiCredentials(WIFI_CREDENTIALS_T credentials){
    preferences.begin(PREFERENCES_NAMESPACE, false);
    preferences.putBytes(CREDENTIALS_KEY, &credentials, sizeof(WIFI_CREDENTIALS_T));
    preferences.end();
}

void deleteWiFiCredentials(){
    preferences.begin(PREFERENCES_NAMESPACE, false);
    preferences.remove(CREDENTIALS_KEY);
    preferences.end();
}

void reconnect(){
    for(
        uint8_t attempts = 0;
        attempts < 3 && !mqttClient.connected();
        attempts++
    ){
        #ifdef DEBUG
        Serial.println("Attempting MQTT connection...");
        #endif

        if(mqttClient.connect(deviceName.c_str())){
            #ifdef DEBUG
            Serial.println("Connected to MQTT server");
            #endif
        }else{
            #ifdef DEBUG
            Serial.print("Failed to connect to MQTT server, rc=");
            Serial.println(mqttClient.state());
            Serial.println("Retrying in 3 seconds...");
            #endif

            delay(3000);
        }
    }

    if(!mqttClient.connected()){
        #ifdef DEBUG
        Serial.println("Failed to connect to MQTT server");
        #endif
    }
}

void registerManager(){
    reconnect();
    uint8_t mac[6];
    WiFi.macAddress(mac);

    if(mqttClient.connected()){
        mqttClient.publish("temptake/manager/register", mac, 6);
    }
}

void registerWorker(String workerMac){
    reconnect();

    if(mqttClient.connected()){
        workerMac.toUpperCase();
        String concatinatedMac = managerMac + workerMac;

        mqttClient.publish("temptake/worker/register", (const uint8_t*)concatinatedMac.c_str(), 24);

        #ifdef DEBUG
        Serial.println("Registered worker: " + workerMac);
        #endif
    }
}

void uploadData(uint8_t* data){
    reconnect();

    if(mqttClient.connected()){
        mqttClient.publish("temptake/entry", data, sizeof(data));

        #ifdef DEBUG
        Serial.println("Uploaded data");
        Serial.print(String(data, HEX));
        #endif
    }
}
