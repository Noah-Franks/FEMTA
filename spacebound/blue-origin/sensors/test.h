#pragma once

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

#define TEST_ADDRESS 0x00         // this one is reserved

#define TEST_MEASURE_ZERO     0   // a stream of zeros
#define TEST_MEASURE_IDENTITY 1   // a stream that always uses its last value
#define TEST_MEASURE_SINE     2   // a sine wave based on time
#define TEST_MEASURE_COSINE   3   // a cosine wave based on time
#define TEST_MEASURE_RANDOM   4   // a stream of random integers
