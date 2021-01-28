
#include "../include/program.h"

local bool read_slf3(i2c_device * slf3_i2c);

Sensor * init_slf3(Sensor * slf3) {
  
  slf3 -> name = "SLF3S-1300F";
  slf3 -> i2c  = create_i2c_device(slf3, read_slf3, "A flowmeter");

  sleep(0.5);
  if (!i2c_write_byte(slf3 -> i2c, 0x36, 0x08)) return false;    // start reading water
  sleep(0.5);
  
  //while (i2cWriteByteData(handle, 0x36, 0x08) < 0) {
  //  sleep(1);
  //}
  
  sensor_log_header(slf3, BLUE);
  return slf3;
}

bool read_slf3(i2c_device * slf3_i2c) {
  
  Sensor * slf3 = slf3_i2c -> sensor;
  float fflow;
  uint8 read_raws[9];
  
  if (!i2c_raw_read(slf3_i2c, read_raws, 9)) return false;

  // Flow rate scale factor: 500 (mL/min)^-1
  int flow = (read_raws[1] | (read_raws[0] << 8));
  if (flow > 32767) {
    fflow = ((read_raws[1] ^ 0x3F) | ((read_raws[0] << 8) ^ 0x3F) | 0x01) / 500.0f;
  }
  else {
    fflow = (read_raws[1] | (read_raws[0] << 8)) / 500.0f;
  }
  // Temperature scale factor: 200 (deg C)^-1
  float temp = (read_raws[4] | (read_raws[3] << 8)) / 200.0f;
  int flag = read_raws[7] | (read_raws[6] << 8);
  
  int bubble = !!(flag & 0b1); // Bubble flag is in LSB of third byte -- 1 = bubble
  
  bind_stream(slf3, fflow , SLF3_MEASURE_FLOW       );
  bind_stream(slf3, temp  , SLF3_MEASURE_TEMPERATURE);
  bind_stream(slf3, bubble, SLF3_MEASURE_BUBBLE     );
  
  sensor_log_outputs(slf3, slf3_i2c -> log, NULL);
  sensor_process_triggers(slf3);
  return true;
}
