#pragma once

/**
 * Sensor:
 *   Accelerometer
 *   ADXL345
 *   Connected via I2C
 * 
 * Datasheet: 
 *   https://www.analog.com/media/en/technical-documentation/data-sheets/adxl345.pdf
 * 
 * Author: 
 *   Noah Franks
 */

#define ADXL_ADDRESS 0x53

#define ADXL_MEASURE_X 0
#define ADXL_MEASURE_Y 1
#define ADXL_MEASURE_Z 2
