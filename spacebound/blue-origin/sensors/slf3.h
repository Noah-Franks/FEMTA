#pragma once

/**
 * Sensor:
 *   High-precision flowmeter for low-volume fluid flows
 *   Sensirion SLF3S-1300F
 *   Connected via I2C
 *
 * Datasheet:
 *   https://www.mouser.com/datasheet/2/682/Sensirion_Liquid_Flow_Sensor_SLF3S-1300F_Datasheet-1621236.pdf
 *
 * Author:
 *   Noah Franks
 */

#define SLF3_MEASURE_FLOW        0
#define SLF3_MEASURE_TEMPERATURE 1
#define SLF3_MEASURE_BUBBLE      2

#define SLF3_ADDRESS 0x08
#define SLF3_AUGMENT 0x09
