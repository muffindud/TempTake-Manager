#include <config.h>
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include "HC12.h"
#include "DataPacker.h"

Adafruit_SSD1306 display(128, 64, &Wire, -1);
HC12 hc12(HC_12_RX_PIN, HC_12_TX_PIN, HC_12_SET_PIN);

DAT_T data_packet;

void setup(){
    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    display.display();

    // Serial.begin(9600);

    delay(500);
}

void loop(){
    data_packet = hc12.receiveData();
    if(data_packet.type == DAT_MODE){
        uint64_t temperature = data_packet.data.temperature;
        Serial.write((uint8_t*)&temperature, sizeof(temperature));

        uint64_t humidity = data_packet.data.humidity;
        Serial.write((uint8_t*)&humidity, sizeof(humidity));

        uint64_t pressure = data_packet.data.pressure;
        Serial.write((uint8_t*)&pressure, sizeof(pressure));

        uint64_t ppm = data_packet.data.ppm;
        Serial.write((uint8_t*)&ppm, sizeof(ppm));

        String data = "";
        data += "T: " + String(temperature) + " C\n";
        data += "H: " + String(humidity) + " %\n";
        data += "P: " + String(pressure) + " mmHg\n";
        data += "PPM: " + String(ppm) + " ppm\n";

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println(data);
        display.display();
    }
}
