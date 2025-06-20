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
bool keyExists(const char* key);
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
    if(strlen(credentials.ssid) == 0 || !keyExists(CREDENTIALS_KEY)){
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

bool keyExists(const char* key){
    preferences.begin(PREFERENCES_NAMESPACE, true);
    bool exists = preferences.isKey(key);
    preferences.end();
    return exists;
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
        managerMac = uintToString(mac);
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
    preferences.clear();
    preferences.end();
}

void reconnect(){
    while (!mqttClient.connected())
    {
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

    #ifdef DEBUG
    Serial.println("Registering manager: " + String(mac[0], HEX) + ":" + String(mac[1], HEX) + ":" + String(mac[2], HEX) + ":" + String(mac[3], HEX) + ":" + String(mac[4], HEX) + ":" + String(mac[5], HEX));
    #endif

    if(mqttClient.connected()){
        mqttClient.publish("temptake/manager/register", mac, sizeof(mac));
    }
}

void registerWorker(uint8_t *workerMac){
    reconnect();

    if(mqttClient.connected()){
        #ifdef DEBUG
        String workerMacString = uintToString(workerMac);
        workerMacString.toUpperCase();
        Serial.println("Registering worker: " + workerMacString);
        #endif

        // First half is manager mac, second half is worker mac in hex
        uint8_t concatinatedMac[12];
        uint8_t mac[6];
        WiFi.macAddress(mac);

        for(int i = 0; i < 6; i++){
            concatinatedMac[i] = mac[i];
        }

        for(int i = 0; i < 6; i++){
            concatinatedMac[i + 6] = workerMac[i];
        }

        mqttClient.publish("temptake/worker/register", concatinatedMac, 12);

        #ifdef DEBUG
        Serial.println("Registered worker: " + workerMacString);
        #endif
    }
}

void uploadData(PACKET_T data){
    reconnect();

    if(mqttClient.connected()){
        mqttClient.publish("temptake/entry", reinterpret_cast<const uint8_t*>(&data), sizeof(data));

        #ifdef DEBUG
        Serial.println("Uploaded data");
        #endif
    }
}
