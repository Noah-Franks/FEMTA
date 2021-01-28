#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pigpio.h>

unsigned int MCP9_ADDRESS = 0x1f;

int read_mcp9(int handle, FILE *log);

int main() {

  int handle, i;
  FILE *log;

  gpioInitialise();
  log = fopen("logs/mcp9.log", "a");
  handle = i2cOpen(1, MCP9_ADDRESS, 0);

  printf("Started MCP9808 on 0x%x\n", MCP9_ADDRESS);

  for (i = 0; i < 20; i++) {
    read_mcp9(handle, log);
    sleep(1);
  }

  fclose(log);
  i2cClose(handle);

  return 0;
}

int read_mcp9(int handle, FILE *log) {
  /*
   * Binary address 0011000; hex address 18
   * Can read max every t_conv, or 250ms w/ 0.0625 *C accuracy
   * Can read max every 30ms w/ 0.5 *C accuracy
   *
   * A high-to-low transition of the SDA line (while SCL is high) is the Start
   * condition. All data transfers must be preceded by a Start condition from
   * the master. A low-to-high transition of the SDA line (while SCL is high)
   * signifies a Stop condition.
   */

  char read_raws[2], upper, lower;
  double temp;

  if (!i2cReadI2CBlockData(handle, 0x05, read_raws, 2)) return 0;

  upper = read_raws[0];
  lower = read_raws[1];

  if (upper & 0x10) {
    upper = upper & 0x0F;  // Mask last 4 bits
    temp = 256 - ((upper << 4) + (lower >> 4));  // Get ambient temp. (-)
    temp -= (lower & 0x0F) * 0.0625f;
  }
  else {
    upper = upper & 0x0F;  // Mask last 4 bits
    temp = (upper << 4) + (lower >> 4);  // Get ambient temp. (+)
    temp += (lower & 0x0F) * 0.0625f;
  }

  printf("%.4f\n", temp);
  fprintf(log, "%.4f\n", temp);

  return 1;
}
