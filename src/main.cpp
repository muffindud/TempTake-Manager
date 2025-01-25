#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <WiFi.h>

#include "HC12.h"
#include "DataPacker.h"
#include "WorkerPair.h"

#include <config.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
HC12 hc12(HC_12_RX_PIN, HC_12_TX_PIN, HC_12_SET_PIN);

DAT_T data_packet;
MAC_ADDRESS_T manager_mac;

void setup(){
    Serial.begin(9600);

    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    display.display();

    WiFi.macAddress(manager_mac.mac);

    Serial.printf("MAC: %2x:%2x:%2x:%2x:%2x:%2x\n",
        manager_mac.mac[0], manager_mac.mac[1], manager_mac.mac[2],
        manager_mac.mac[3], manager_mac.mac[4], manager_mac.mac[5]
    );

    pinMode(PAIR_BUTTON_PIN, INPUT_PULLUP);
    delay(500);
}

void loop(){
    if(digitalRead(PAIR_BUTTON_PIN) == LOW){
        pairWorker(manager_mac);
    }

    data_packet = hc12.receiveData();
    if(data_packet.type == DAT_MODE){
        String data = "";
        data += "T: " + String(data_packet.data.temperature / 100. - 40.) + " C\n";
        data += "H: " + String(data_packet.data.humidity / 100.) + " %\n";
        data += "P: " + String(data_packet.data.pressure / 100.) + " mmHg\n";
        data += "PPM: " + String(data_packet.data.ppm / 100.) + " ppm\n";

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println(data);
        display.display();

        // Will upload the data using MQTT
    }
}
