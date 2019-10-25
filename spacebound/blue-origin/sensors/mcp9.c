#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "mcp9.h"

#include "../system/color.h"
#include "../system/gpio.h"
#include "../system/i2c.h"

void free_mcp9(Sensor * sensor);
bool read_mcp9(i2c_device * mcp9_i2c);

Sensor * init_mcp9(ProtoSensor * proto) {

  Sensor * mcp9 = sensor_from_proto(proto);

  mcp9 -> name = "MCP9808";
  mcp9 -> free = free_mcp9;

  mcp9 -> i2c = create_i2c_device(mcp9, proto -> address, read_mcp9, proto -> hertz);
  mcp9 -> i2c -> log = fopen("logs/mcp9.log", "a");
  
  mcp9 -> print = proto -> print;

  fprintf(mcp9 -> i2c -> log, GREEN "\nMCP9808\n" RESET);
  printf("Started " GREEN "%s " RESET "at " YELLOW "%dHz " RESET "on " BLUE "0x%x " RESET,
	 mcp9 -> name, proto -> hertz, proto -> address);

  printf("logged in logs/mcp9.log\n");
  
  return mcp9;

  return NULL;
}

bool read_mcp9(i2c_device * mcp9_i2c) {
  /*
   * Binary address 0011111; hex address 1F
   * Can read max every t_conv, or 250ms w/ 0.0625 *C accuracy
   * Can read max every 30ms w/ 0.5 *C accuracy
   *
   * A high-to-low transition of the SDA line (while SCL is high) is the Start
   * condition. All data transfers must be preceded by a Start condition from
   * the master. A low-to-high transition of the SDA line (while SCL is high)
   * signifies a Stop condition.
   */
  
  Sensor * mcp9 = mcp9_i2c -> sensor;

  uint8 read_raws[2], upper, lower;
  double temp;
  
  if (!i2c_read_bytes(mcp9_i2c, 0x05, read_raws, 2)) return false;

  upper = read_raws[0];
  lower = read_raws[1];

  if (upper & 0x10) {
    upper = upper & 0x0F;  // Mask last 4 bits
    temp = 256 - ((upper << 4) + (lower >> 4));  // Get ambient temp. (-)
    temp -= (lower & 0x0F) * 0.0625f;            // Get decimal value
  }
  else {
    upper = upper & 0x0F;  // Mask last 4 bits
    temp = upper << 4 + lower >> 4;    // Get ambient temp. (+)
    temp += (lower & 0x0F) * 0.0625f;  // Get decimal value
  }

  fprintf(mcp9 -> i2c -> log, "%.4f\n", temp);
  
  return true;
}

void free_mcp9(Sensor * ds32) {
  // Times out and resets after 25-35 ms of no change on SDA/SCL lines
}
