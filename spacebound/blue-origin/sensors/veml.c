
#include "../include/program.h"

local bool read_veml(i2c_device * veml_i2c);    // how to read from this sensor

Sensor * init_veml(Sensor * veml) {
  
  veml -> name = "VEML6075";
  veml -> i2c = create_i2c_device(veml, read_veml, "A photodiode");
  
  sensor_log_header(veml, MAGENTA);
  return veml;
}

bool read_veml(i2c_device * veml_i2c) {
  
  Sensor * veml = veml_i2c -> sensor;
  
  if (!i2c_write_byte(veml_i2c, 0x00, 0b01000110)) return false;
  
  uint8 read_raws[4];
  
  if (!i2c_read_bytes(veml_i2c, 0x07, read_raws    , 2)) return false;
  if (!i2c_read_bytes(veml_i2c, 0x09, read_raws + 2, 2)) return false;
  
  int uva = read_raws[0] | (read_raws[1] << 8);
  int uvb = read_raws[2] | (read_raws[3] << 8);
  
  bind_stream(veml, uva, VEML_MEASURE_UVA);
  bind_stream(veml, uvb, VEML_MEASURE_UVB);
  
  sensor_log_outputs(veml, veml_i2c -> log, NULL);
  sensor_process_triggers(veml);
  return true;
}
