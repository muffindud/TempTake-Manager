#include "WorkerPairLib.h"

bool pinSetup = false;
const char hexChars[] = "0123456789ABCDEF";

bool isPairingMode(){
    if(!pinSetup){
        pinMode(PAIR_BUTTON_PIN, INPUT_PULLUP);
        pinSetup = true;
    }

    return digitalRead(PAIR_BUTTON_PIN) == LOW;
}

String uintToString(uint8_t* mac){
    static char macAddressString[12];
    for (int i = 0; i < 6; i++) {
        macAddressString[i * 2]     = hexChars[(mac[i] >> 4) & 0x0F];
        macAddressString[i * 2 + 1] = hexChars[mac[i] & 0x0F];
    }

    return String(macAddressString);
}

uint8_t* exchangeWorkerCreds(){
    uint8_t managerMac[6];
    static uint8_t workerMac[6];

    memset(workerMac, 0, sizeof(workerMac));

    esp_efuse_mac_get_default(managerMac);

    delay(100);

    #ifdef DEBUG
    Serial.println("Sending manager mac: " + uintToString(managerMac));
    #endif

    Wire.beginTransmission(PAIR_ADDR);
    Wire.write(managerMac, 6);
    Wire.endTransmission();

    delay(100);

    Wire.requestFrom(PAIR_ADDR, 6);
    if(Wire.available() == 6){
        for(int i = 0; i < 6; i++){
            workerMac[i] = Wire.read();
        }
    }

    return workerMac;
}
