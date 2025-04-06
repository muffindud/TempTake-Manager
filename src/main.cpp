#include <Arduino.h>

#include "NetworkLib.h"
#include "DataPackerLib.h"
#include "HC12Lib.h"
#include "WorkerPairLib.h"

#include "config.h"

#ifdef DISPLAY_ON
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#endif

#ifdef DISPLAY_ON
Adafruit_SSD1306 display(128, 64, &Wire, -1);
#endif

HC12 hc12 = HC12(HC_12_RX_PIN, HC_12_TX_PIN, HC_12_SET_PIN);
bool pairingStateHolder = false;

void setup(){
    #ifdef DEBUG
    Serial.begin(115200);
    #endif

    #ifdef DISPLAY_ON
    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
    display.display();
    #endif

    while(!initWiFi()){}
}

void loop(){
    if(isPairingMode()){
        if(!pairingStateHolder){
            uint8_t* workerMac = exchangeWorkerCreds();
            registerWorker(uintToString(workerMac));

            // Set state on to avoid repeated execution
            pairingStateHolder = true;
        }
    }else{
        // Reset state
        pairingStateHolder = false;

        PACKET_T packet = hc12.receiveData();
        if(packet.type == DAT_MODE){
            #ifdef DISPLAY_ON
            String data_string = "";
            data_string += "T: " + String(packet.data.temperature / 100. - 40.) + " C\n";
            data_string += "H: " + String(packet.data.humidity / 100.) + " %\n";
            data_string += "P: " + String(packet.data.pressure / 100.) + " mmHg\n";
            data_string += "PPM: " + String(packet.data.ppm / 100.) + " ppm\n";

            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 0);
            display.println(data_string);
            display.display();

            #ifdef DEBUG
            Serial.println(data_string);
            #endif
            #endif

            uploadData((uint8_t*)&packet);
        }
    }
}
