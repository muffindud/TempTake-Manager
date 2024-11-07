#include <config.h>
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <SoftwareSerial.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
SoftwareSerial HC12(HC_12_RX_PIN, HC_12_TX_PIN);

String data = "";
String oldData = "";
int delaySeconds = 1;

void setup(){
    Serial.begin(9600);
    HC12.begin(HC_12_BAUD_RATE);

    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    display.display();

    delay(500);
}

void loop(){
    // if(Serial.available()){
    //     HC12.write(Serial.read());
    // }
    // if(HC12.available()){
    //     Serial.write(HC12.read());
    // }

    if(HC12.available()){
        data = HC12.readString();
        Serial.println(data);
        if(data != oldData){
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 0);
            display.println(data);
            display.display();
            oldData = data;
        }
    }
}
