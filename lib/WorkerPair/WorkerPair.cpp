#include "WorkerPair.h"

void pairWorker(MAC_ADDRESS_T manager_mac){
    MAC_ADDRESS_T worker_mac;

    Wire.begin();

    Wire.beginTransmission(PAIR_ADDR);
    Wire.write(manager_mac.mac, 6);
    Wire.endTransmission();

    delay(100);

    Wire.requestFrom(PAIR_ADDR, 6);
    while(Wire.available()){
        Wire.readBytes(worker_mac.mac, 6);
    }

    addWorkerMac(worker_mac);

    return;
}

void addWorkerMac(MAC_ADDRESS_T worker_mac){
    // Will upload the MAC address to the database using MQTT

    Serial.print("Added worker MAC: ");
    for(int i = 0; i < 6; i++){
        Serial.print(worker_mac.mac[i], HEX);
        Serial.print(":");
    }
    Serial.println();

    return;
}
