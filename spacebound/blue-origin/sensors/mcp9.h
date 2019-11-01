#ifndef HEADER_GUARD_MCP9
#define HEADER_GUARD_MCP9

/**
 * Sensor:
 *   0.5C Maximum Accuracy Digital Temperature Sensor
 *   MCP9808
 *   Connected via I2C
 *
 * Datasheet:
 *   http://ww1.microchip.com/downloads/en/DeviceDoc/25095A.pdf
 *
 * License:
 *   GPLv3
 *
 * Author:
 *   Mark Hartigan
 */

#include "sensor.h"
#include "../system/i2c.h"

#define MCP9_ADDRESS 0x1f

Sensor * init_mcp9(Sensor * proto);
bool read_mcp9(i2c_device * mcp9_i2c);
void free_mcp9(Sensor * ds32);

#endif

