
#include "../include/program.h"

local void free_mcp9(Sensor * sensor);
local bool read_mcp9(i2c_device * mcp9_i2c);

Sensor * init_mcp9(Sensor * mcp9) {
  
  mcp9 -> name = "MCP9808";
  mcp9 -> free = free_mcp9;
  
  mcp9 -> i2c = create_i2c_device(mcp9, read_mcp9);
  mcp9 -> i2c -> log = safe_open("logs/mcp9.log", "a");
  
  printf("logged in logs/mcp9.log\n");
  printf("An ambient thermometer\n\n");
  
  
  /* set up output data stream */
  
  Output * output = &mcp9 -> outputs[MCP9_MEASURE_TEMPERATURE];
  
  output -> enabled = true;
  
  fprintf(mcp9 -> i2c -> log, RED "\n\nMCP9\nStart time %s\nTime [%s]\tTemperature [%s]\n" RESET,
	  formatted_time, time_unit, output -> unit);
  
  return mcp9;
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
  
  uint8 read_raws[2];
  double temperature;
  
  if (!i2c_read_bytes(mcp9_i2c, 0x05, read_raws, 2)) return false;
  
  u8 upper = read_raws[0];
  u8 lower = read_raws[1];
  
  if (upper & 0x10) {
    upper = upper & 0x0F;  // Mask last 4 bits
    temperature = 256 - ((upper << 4) + (lower >> 4));  // Get ambient temp. (-)
    temperature -= (lower & 0x0F) * 0.0625f;            // Get decimal value
  }
  else {
    upper = upper & 0x0F;  // Mask last 4 bits
    temperature = (upper << 4) + (lower >> 4);          // Get ambient temp. (+)
    temperature += (lower & 0x0F) * 0.0625f;            // Get decimal value
  }
  
  
  /* collected raw values, now to assign output streams */
  
  Output * output = &mcp9 -> outputs[MCP9_MEASURE_TEMPERATURE];
  
  output -> measure = series_compute(output -> series, temperature);
  
  if (mcp9 -> print)
    printf("%s%s      %.5fs    %.4f%s\n",
	   mcp9 -> print_code, mcp9 -> code_name, time_passed(), output -> measure, output -> unit);
  
  fprintf(mcp9_i2c -> log, "%.5f\t%.5f\n", time_passed(), temperature);
  
  sensor_process_triggers(mcp9);
  return true;
}

void free_mcp9(Sensor * ds32) {
  // Times out and resets after 25-35 ms of no change on SDA/SCL lines
}
