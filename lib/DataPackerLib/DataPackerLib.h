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

/**
 * Gets the ID of a packet
 *
 * @param source_mac The MAC address of the source
 * @param dest_mac The MAC address of the destination
 * @param data_p The data of the packet
 * @param crc16 The CRC16 of the packet
 * @returns The ID of the packet
 */
uint32_t getId(uint8_t* source_mac, uint8_t* dest_mac, DATA_T data_p, uint16_t crc16);

/**
 * Gets the CRC16 of a packet using the modbus algorithm
 *
 * @param data The data of the packet
 * @returns The CRC16 of the packet
 */
uint16_t getCRC16(DATA_T data);

#ifdef WORKER
/**
 * @brief Builds a packet
 *
 * @param manager_mac The MAC address of the manager
 * @param worker_mac The MAC address of the worker
 * @param temperature_c The temperature in Celsius
 * @param humidity_perc The humidity in percentage
 * @param pressure_mmHg The pressure in mmHg
 * @param ppm The PPM
 * @returns The built packet
 */
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
/**
 * Casts a buffer to a PACKET_T struct
 *
 * @param buffer The buffer to cast
 * @returns The cast PACKET_T struct
 */
PACKET_T castPacket(uint8_t* buffer);
#endif

#endif
