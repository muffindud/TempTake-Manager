#include "WiFiManager.h"

WiFiClient espClient;
PubSubClient client(espClient);
Preferences preferences;
String ssid;
String pass;

bool initWiFi(String mqtt_server, uint16_t mqtt_port){
    preferences.begin("wifi", false);

    bool credentialsPresent = loadWiFiCredentials();

    if(!credentialsPresent){
        listenForCredentials();
    }

    WiFi.begin(ssid.c_str(), pass.c_str());

    #ifdef DEBUG
    Serial.print("Connecting to " + String(ssid));
    #endif

    uint8_t attempts = 0;
    while(WiFi.status() != WL_CONNECTED && attempts < 10){
        attempts++;
        delay(1000);

        #ifdef DEBUG
        Serial.print(".");
        #endif
    }

    if(WiFi.status() != WL_CONNECTED){
        #ifdef DEBUG
        Serial.println("Failed to connect to WiFi");
        #endif

        if(credentialsPresent){
            eraseWiFiCredentials();
        }

        return false;
    }

    if(!credentialsPresent){
        saveWiFiCredentials();
    }

    #ifdef DEBUG
    Serial.println();
    Serial.println("Connected to WiFi");
    Serial.println("IP: " + WiFi.localIP().toString());
    #endif

    client.setServer(mqtt_server.c_str(), mqtt_port);

    #ifdef DEBUG
    uint8_t mac[6];
    WiFi.macAddress(mac);
    Serial.printf("MAC: %2x:%2x:%2x:%2x:%2x:%2x\n",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]
    );
    #endif

    return true;
}

bool loadWiFiCredentials(){
    ssid = preferences.getString("ssid", "");

    if(ssid == ""){
        return false;
    }

    pass = preferences.getString("pass", "");

    #ifdef DEBUG
    if(ssid != ""){
        Serial.println("Loaded WiFi credentials");
        Serial.println("SSID: " + ssid);
    }

    if(pass != ""){
        Serial.println("PASS: " + pass);
    }
    #endif

    return ssid != "";
}

void listenForCredentials(){
    BluetoothSerial SerialBT;
    String device_name = "TempTake Manager " + String(ESP.getEfuseMac(), HEX);
    SerialBT.begin(device_name);

    #ifdef DEBUG
    Serial.println("Listening for WiFi credentials");
    #endif

    while(true){
        if(SerialBT.available()){
            String data = SerialBT.readStringUntil('\n');
            data.trim();

            int separator = data.indexOf(':');

            if(separator != -1){
                ssid = data.substring(0, separator);
                pass = data.substring(separator + 1);
            }else{
                ssid = data;
                pass = "";
            }

            // #ifdef DEBUG
            Serial.println("Received WiFi credentials");
            Serial.println("SSID: " + ssid);
            Serial.println("PASS: " + pass);
            // #endif

            SerialBT.end();
            break;
        }
    }
}

void saveWiFiCredentials(){
    preferences.putString("ssid", ssid);
    preferences.putString("pass", pass);
}

void eraseWiFiCredentials(){
    preferences.clear();

    #ifdef DEBUG
    Serial.println("Erased WiFi credentials");
    #endif
}
