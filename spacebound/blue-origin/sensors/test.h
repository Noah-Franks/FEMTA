#ifndef HEADER_GUARD_TEST
#define HEADER_GUARD_TEST

/**
 * Sensor:
 *   A test sensor providing virtual data streams.
 *
 * License:
 *   GPLv3
 *
 * Author:
 *   Noah Franks
 */

#include "sensor.h"
#include "../system/i2c.h"

#define TEST_ADDRESS 0x00         // this one is reserved

#define TEST_MEASURE_ZERO     0   // a stream of zeros
#define TEST_MEASURE_IDENTITY 1   // a stream that always uses its last value
#define TEST_MEASURE_SINE     2   // a sine wave based on time
#define TEST_MEASURE_COSINE   3   // a cosine wave based on time
#define TEST_MEASURE_RANDOM   4   // a stream of random integers

Sensor * init_test(Sensor * proto);
bool read_test(i2c_device * mcp9_i2c);
void free_test(Sensor * test);

#endif
