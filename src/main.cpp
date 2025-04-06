#include <Arduino.h>

#include "NetworkLib.h"
#include "DataPackerLib.h"
#include "HC12Lib.h"
#include "WorkerPairLib.h"

#include "config.h"

HC12 hc12 = HC12(HC_12_RX_PIN, HC_12_TX_PIN, HC_12_SET_PIN);
bool pairingStateHolder = false;

void setup(){
    #ifdef DEBUG
    Serial.begin(115200);
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


    }
}
