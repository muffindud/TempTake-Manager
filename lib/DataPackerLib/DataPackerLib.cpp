#include "DataPackerLib.h"

#ifdef WORKER
PACKET_T buildPacket(
    uint8_t *manager_mac,
    uint8_t *worker_mac,
    uint64_t temperature_c,
    uint64_t humidity_perc,
    uint64_t pressure_mmHg,
    uint64_t ppm
){
    PACKET_T packet;
    packet.type = DAT_MODE;
    memcpy(packet.meta.manager_mac, manager_mac, 6);
    memcpy(packet.meta.worker_mac, worker_mac, 6);
    packet.meta.index_packet = 0;
    packet.meta.total_packet = 1;
    packet.length = DATA_SIZE;
    packet.data.temperature = (uint64_t)((temperature_c + 40.) * 100.);
    packet.data.humidity = (uint64_t)(humidity_perc * 100.);
    packet.data.pressure = (uint64_t)(pressure_mmHg * 100.);
    packet.data.ppm = (uint64_t)(ppm * 100.);
    packet.meta.crc16 = getCRC16(packet.data);
    packet.meta.packet_id = getId(worker_mac, manager_mac, packet.data, packet.meta.crc16);

    return packet;
}
#endif

#ifdef MANAGER
PACKET_T castPacket(uint8_t* buffer){
    PACKET_T packet;
    memcpy(&packet, buffer, PACKET_SIZE);

    return packet;
}
#endif

uint32_t hash32(uint8_t* data, size_t length) {
    uint32_t hash = 0x811c9dc5;
    for (size_t i = 0; i < length; i++) {
        hash ^= data[i];
        hash *= 0x01000193;
    }

    return hash;
}

uint32_t getId(uint8_t* worker_mac, uint8_t* manager_mac, DATA_T data_p, uint16_t crc16) {
    uint8_t combined_mac[12];
    for (int i = 0; i < 6; i++) {
        combined_mac[i] = worker_mac[i];
        combined_mac[i + 6] = manager_mac[i];
    }

    uint32_t mac_hash = hash32(combined_mac, 12);

    return (mac_hash ^ crc16);
}

uint16_t getCRC16(DATA_T data_p){
    uint8_t* data = (uint8_t*)&data_p;
    uint16_t crc = 0xFFFF;
    for(int i = 0; i < DATA_SIZE; i++){
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

bool checkData(PACKET_T packet)
{
    uint16_t crc = getCRC16(packet.data);
    if (crc != packet.meta.crc16) {
        #ifdef DEBUG
        Serial.println("CRC mismatch: expected " + String(packet.meta.crc16, HEX) + ", got " + String(crc, HEX));
        #endif
        return false;
    }

    uint32_t id = getId(packet.meta.worker_mac, packet.meta.manager_mac, packet.data, packet.meta.crc16);
    if (id != packet.meta.packet_id) {
        #ifdef DEBUG
        Serial.println("ID mismatch: expected " + String(packet.meta.packet_id, HEX) + ", got " + String(id, HEX));
        #endif
        return false;
    }

    return true;
}
