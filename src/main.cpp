#include <Arduino.h>
#include <Wire.h>

#include "NetworkLib.h"
#include "DataPackerLib.h"
#include "HC12Lib.h"
#include "WorkerPairLib.h"

#include "config.h"

#ifdef DISPLAY_ON
#include <Adafruit_SSD1306.h>
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

    Wire.begin();

    #ifdef DISPLAY_ON
    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR, true, false);
    display.display();
    #endif

    while(!initWiFi()){
        deleteWiFiCredentials();
    }
}

void loop(){
    if(isPairingMode()){
        if(!pairingStateHolder){
            uint8_t* workerMac = exchangeWorkerCreds();
            registerWorker(workerMac);

            // Set state on to avoid repeated execution
            pairingStateHolder = true;
        }
    }else{
        // Reset state
        pairingStateHolder = false;

        PACKET_T packet = hc12.receiveData();
        if(packet.type == DAT_MODE){
            #if defined(DEBUG) || defined(DISPLAY_ON)
            String data_string = "";
            data_string += "ID: " + String(packet.meta.packet_id) + "\n\r";
            data_string += "M: " + String(packet.meta.manager_mac[0], HEX) + ":" + String(packet.meta.manager_mac[1], HEX) + ":" + String(packet.meta.manager_mac[2], HEX) + ":" + String(packet.meta.manager_mac[3], HEX) + ":" + String(packet.meta.manager_mac[4], HEX) + ":" + String(packet.meta.manager_mac[5], HEX) + "\n\r";
            data_string += "W: " + String(packet.meta.worker_mac[0], HEX) + ":" + String(packet.meta.worker_mac[1], HEX) + ":" + String(packet.meta.worker_mac[2], HEX) + ":" + String(packet.meta.worker_mac[3], HEX) + ":" + String(packet.meta.worker_mac[4], HEX) + ":" + String(packet.meta.worker_mac[5], HEX) + "\n\r";
            data_string += "T: " + String(packet.data.temperature / 100. - 40.) + " C\n\r";
            data_string += "H: " + String(packet.data.humidity / 100.) + " %\n\r";
            data_string += "P: " + String(packet.data.pressure / 100.) + " mmHg\n\r";
            data_string += "PPM: " + String(packet.data.ppm / 100.) + " ppm\n\r";
            #endif

            #ifdef DISPLAY_ON
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

            if (checkData(packet)) {
                uploadData(packet);
            }
        }
    }
}
