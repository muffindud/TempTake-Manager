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

uint32_t hash32(uint8_t* data, size_t length) {
    uint32_t hash = 0x811c9dc5;
    for (size_t i = 0; i < length; i++) {
        hash ^= data[i];
        hash *= 0x01000193;
    }

    return hash;
}

uint32_t getId(uint8_t* source_mac, uint8_t* dest_mac, RAW_DATA_T data_p, uint16_t crc16) {
    uint8_t combined_mac[12];
    for (int i = 0; i < 6; i++) {
        combined_mac[i] = source_mac[i];
        combined_mac[i + 6] = dest_mac[i];
    }

    uint32_t mac_hash = hash32(combined_mac, 12);

    return (mac_hash ^ crc16);
}
