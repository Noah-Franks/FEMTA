
#include "../include/program.h"

local bool read_slf3(i2c_device * slf3_i2c);

Sensor * init_slf3(Sensor * slf3) {
  
  slf3 -> name = "SLF3S-1300F";
  slf3 -> i2c  = create_i2c_device(slf3, read_slf3, "A flowmeter");
  
  if (!i2c_write_byte(slf3 -> i2c, 0x36, 0x08)) return false;    // start reading water
  
  sensor_log_header(slf3, BLUE);
  return slf3;
}

bool read_slf3(i2c_device * slf3_i2c) {
  
  Sensor * slf3 = slf3_i2c -> sensor;
  
  uint8 read_raws[9];
  
  if (!i2c_raw_read(slf3_i2c, read_raws, 9)) return false;
  
  int flow = read_raws[1] | (read_raws[0] << 8);
  int temp = read_raws[4] | (read_raws[3] << 8);
  int flag = read_raws[7] | (read_raws[6] << 8);
  
  int bubble = !!(flag & 0b1);
  
  bind_stream(slf3, flow  , SLF3_MEASURE_FLOW       );
  bind_stream(slf3, temp  , SLF3_MEASURE_TEMPERATURE);
  bind_stream(slf3, bubble, SLF3_MEASURE_BUBBLE     );
  
  sensor_log_outputs(slf3, slf3_i2c -> log, NULL);
  sensor_process_triggers(slf3);
  return true;
}
