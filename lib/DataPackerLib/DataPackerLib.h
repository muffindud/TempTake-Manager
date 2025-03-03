#ifndef DATAPACKERLIB_H
#define DATAPACKERLIB_H

#include <stdint.h>
#include <string.h>

#include "config.h"

#define DAT_MODE 0xDD
#define ACK_MODE 0xAA

#pragma pack(push, 1)
typedef struct {
    uint16_t crc16;
    uint8_t manager_mac[6];
    uint8_t worker_mac[6];
    uint32_t packet_id;
    uint8_t index_packet;
    uint8_t total_packet;
} META_T;
#pragma pack(pop)
const uint8_t META_SIZE = sizeof(META_T);

#pragma pack(push, 1)
typedef struct {
    uint64_t temperature;
    uint64_t humidity;
    uint64_t pressure;
    uint64_t ppm;
} DATA_T;
#pragma pack(pop)
const uint8_t DATA_SIZE = sizeof(DATA_T);

#pragma pack(push, 1)
typedef struct {
    uint8_t type;
    META_T meta;
    uint8_t length;
    DATA_T data;
} PACKET_T;
#pragma pack(pop)
const uint8_t PACKET_SIZE = sizeof(PACKET_T);

uint32_t getId(uint8_t* source_mac, uint8_t* dest_mac, DATA_T data_p, uint16_t crc16);
uint16_t getCRC16(DATA_T data);

#ifdef WORKER
PACKET_T buildPacket(
    uint8_t *manager_mac,
    uint8_t *worker_mac,
    uint64_t temperature_c,
    uint64_t humidity_perc,
    uint64_t pressure_mmHg,
    uint64_t ppm
);
#endif

#ifdef MANAGER
PACKET_T castPacket(uint8_t* buffer);
#endif

#endif
