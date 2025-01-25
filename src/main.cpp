#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <esp_wifi.h>

#include "HC12.h"
#include "DataPacker.h"
#include "WorkerPair.h"

#include "config.h"

Adafruit_SSD1306 display(128, 64, &Wire, -1);
HC12 hc12(HC_12_RX_PIN, HC_12_TX_PIN, HC_12_SET_PIN);

DAT_T data_packet;
MAC_ADDRESS_T manager_mac;

void setup(){
    Wire.begin();

    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    display.display();

    esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, manager_mac.mac);

    Serial.begin(9600);

    Serial.print("MAC address: ");
    for(int i = 0; i < 6; i++){
        Serial.print(manager_mac.mac[i], HEX);
        Serial.print(":");
    }

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
