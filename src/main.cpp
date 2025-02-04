#include <Arduino.h>
#include <SoftwareSerial.h>

#include "HC12.h"
#include "DataPacker.h"
#include "WorkerPair.h"
#include "WiFiManager.h"

#include <config.h>

#ifdef DISPLAY_ON
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#endif


#ifdef DISPLAY_ON
Adafruit_SSD1306 display(128, 64, &Wire, -1);
#endif

DAT_T data_packet;
RAW_DATA_T data;
MAC_ADDRESS_T manager_mac;

HC12 hc12(HC_12_RX_PIN, HC_12_TX_PIN, HC_12_SET_PIN);


void reconnect(){
    while(!client.connected()){
        #ifdef DEBUG
        Serial.print("Attempting MQTT connection...");
        #endif

        if(client.connect("ESP32Client")){
            #ifdef DEBUG
            Serial.println("connected");
            #endif
        }else{
            #ifdef DEBUG
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            #endif

            delay(5000);
        }
    }
}

bool uploadData(){
    if(!client.connected()){
        client.connect("ESP32Client");
    }

    client.publish("temptake/manager", (const uint8_t*)&data_packet, (unsigned int)sizeof(data_packet));
    return true;
}

void setup(){
    #ifdef DEBUG
    Serial.begin(9600);
    #endif

    while(!initWiFi(MQTT_SERVER, MQTT_PORT)){
        delay(1000);
    }

    WiFi.macAddress(manager_mac.mac);

    #ifdef DEBUG
    Serial.printf("MAC: %2x:%2x:%2x:%2x:%2x:%2x\n",
        manager_mac.mac[0], manager_mac.mac[1], manager_mac.mac[2],
        manager_mac.mac[3], manager_mac.mac[4], manager_mac.mac[5]
    );
    #endif

    #ifdef DISPLAY_ON
    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    display.display();
    #endif

    pinMode(PAIR_BUTTON_PIN, INPUT_PULLUP);
    delay(500);
}

void loop(){
    if(digitalRead(PAIR_BUTTON_PIN) == LOW){
        pairWorker(manager_mac);
    }

    data_packet = hc12.receiveData();
    if(data_packet.type == DAT_MODE){
        memcpy(&data, data_packet.data, sizeof(data));

        #ifdef DISPLAY_ON
        String data_string = "";
        data_string += "T: " + String(data.temperature / 100. - 40.) + " C\n";
        data_string += "H: " + String(data.humidity / 100.) + " %\n";
        data_string += "P: " + String(data.pressure / 100.) + " mmHg\n";
        data_string += "PPM: " + String(data.ppm / 100.) + " ppm\n";

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println(data_string);
        display.display();
        #endif

        #ifdef DEBUG
        Serial.println(data_string);
        #endif

        bool result = uploadData();

        // TODO: Check for success
    }
}
