#pragma once

/**
 * Sensor:
 *   Real Time Clock with Temperature-compensated Crystal Oscillator
 *   RTC TCXO DS3231N
 *   Connected via I2C
 * 
 * Datasheet: 
 *   https://datasheets.maximintegrated.com/en/ds/DS3231.pdf
 * 
 * License:
 *   GPLv3   
 * 
 * Author: 
 *   Noah Franks
 */

#include "../include/headers.h"

#define DS32_MEASURE_TIME        0
#define DS32_MEASURE_TEMPERATURE 1

#define DS32_ADDRESS 0x68

extern char formatted_time[32];
