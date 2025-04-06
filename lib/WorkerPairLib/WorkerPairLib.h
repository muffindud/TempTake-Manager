#ifndef WORKERPAIRLIB_H
#define WORKERPAIRLIB_H

#include <Arduino.h>
#include <Wire.h>

#define PAIR_ADDR 0x55
#define PAIR_BUTTON_PIN 19

#include "config.h"

/**
 * Check if the module is in pairing mode
 *
 * @returns `true` if PAIR_BUTTON_PIN is LOW, `false` if otherwise
 */
bool isPairingMode();

/**
 * Convert a uint8_t arr to a String for sending
 *
 * @param mac pointer to the mac addr
 * @returns converted string
 */
String uintToString(uint8_t* mac);

/**
 * Exchange credentials with the worker module
 *
 * @returns pointer to the worker mac address
 */
uint8_t* exchangeWorkerCreds();

#endif
