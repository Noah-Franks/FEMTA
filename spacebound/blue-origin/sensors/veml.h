#pragma once

/**
 * Sensor:
 *   Ultraviolet Photodiode Sensor Apparatus measuring 300-400nm wavelengths
 *   Vishay Semiconductors VEML6075
 *   Connected via I2C
 * 
 * Datasheet:
 *   https://cdn.sparkfun.com/assets/3/c/3/2/f/veml6075.pdf
 * 
 * Author:
 *   Noah Franks
 */

#define VEML_MEASURE_UVA  0
#define VEML_MEASURE_UVB  1

#define VEML_ADDRESS 0x10
