#pragma once

/**
 * Sensor:
 *   A virtual sensor for the ARM v6 l processor on the raspberry pi.
 *   Measures both resource statistics and cpu temperature.
 * 
 * Author:
 *   Noah Franks
 */

#define ARM6_MEASURE_LOAD        0   // the cpu usage
#define ARM6_MEASURE_MEMORY      1   // the ram usage
#define ARM6_MEASURE_TEMPERATURE 2   // the cpu temperature
