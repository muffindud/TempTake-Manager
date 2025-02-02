#include "DataPacker.h"

uint16_t crc16(RAW_DATA_T data_p){
    uint8_t* data = (uint8_t*)&data_p;
    uint16_t crc = 0xFFFF;
    for(int i = 0; i < RAW_DATA_SIZE; i++){
        crc ^= data[i];

        for(int j = 0; j < 8; j++){
            if(crc & 0x0001){
                crc >>= 1;
                crc ^= 0xA001;
            }else{
                crc >>= 1;
            }
        }
    }

    return crc;
}

uint16_t getId(uint8_t* source_mac, uint8_t* dest_mac, RAW_DATA_T data_p, uint16_t crc16) {
    uint16_t id = 0x0;
    uint8_t* data = (uint8_t*)&data_p;
    int mac_idx = data[RAW_DATA_SIZE / 2] % 6;

    id |= ((source_mac[mac_idx] + dest_mac[mac_idx]) & 0xFF) << 8;
    id |= ((crc16 & 0xFF) + ((crc16 & 0xFF00) >> 8));

    return id;
}
