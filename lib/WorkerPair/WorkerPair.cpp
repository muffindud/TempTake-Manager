#include "WorkerPair.h"

void pairWorker(MAC_ADDRESS_T manager_mac, PubSubClient client){
    MAC_ADDRESS_T worker_mac;

    Wire.begin();

    Wire.beginTransmission(PAIR_ADDR);
    Wire.write(manager_mac.mac, 6);
    Wire.endTransmission();

    delay(100);

    Wire.requestFrom(PAIR_ADDR, 6);
    if(Wire.available() == 6){
        for(int i = 0; i < 6; i++){
            worker_mac.mac[i] = Wire.read();
        }
    }

    addWorkerMac(worker_mac, client);
}

void addWorkerMac(MAC_ADDRESS_T worker_mac, PubSubClient client){
    // Will upload the MAC address to the database using MQTT
    if(!client.connected()){
        client.connect("ESP32Client");
    }

    MAC_ADDRESS_T manager_mac;
    WiFi.macAddress(manager_mac.mac);
    for(int i = 0; i < 6; i++){
        manager_mac.mac[i] = toupper(manager_mac.mac[i]);
    }

    String concatinated_mac = "";
    for(int i = 0; i < 6; i++){
        concatinated_mac += manager_mac.mac[i];
    }
    for(int i = 0; i < 6; i++){
        concatinated_mac += worker_mac.mac[i];
    }

    client.publish("temptake/worker/register", (const uint8_t*)concatinated_mac.c_str(), (unsigned int)concatinated_mac.length());

    Serial.printf("Added worker MAC: %2x:%2x:%2x:%2x:%2x:%2x\n\r",
        worker_mac.mac[0], worker_mac.mac[1], worker_mac.mac[2],
        worker_mac.mac[3], worker_mac.mac[4], worker_mac.mac[5]
    );
}
