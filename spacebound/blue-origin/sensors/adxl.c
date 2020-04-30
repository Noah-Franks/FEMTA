
#include "../include/program.h"

local bool read_adxl(i2c_device * adxl_i2c);

Sensor * init_adxl(Sensor * adxl) {
  
  adxl -> name = "ADXL345";
  adxl -> i2c = create_i2c_device(adxl, read_adxl, "An accelerometer");
  
  sensor_log_header(adxl, RED);
  
  // set up data format for acceleration measurements (page 28)
  // tell adxl to use full resolution when measuring acceleration (bit    3)
  // tell adxl to use full range      when measuring acceleration (bits 1,2)
  i2c_write_byte(adxl -> i2c, 0x31, 0b00001011);    
  
  // tell adxl to bypass it's FIFO queue (page 28)
  i2c_write_byte(adxl -> i2c, 0x38, 0b00000000);
  
  // tell adxl to enter measurement mode (page 26)
  i2c_write_byte(adxl -> i2c, 0x2D, 0b00001000);    // bit 3 indicates measure mode
  
  return adxl;
}

bool read_adxl(i2c_device * adxl_i2c) {
  
  Sensor * adxl = adxl_i2c -> sensor;
  
  uint8 accel_raws[6];
  
  if (!i2c_read_bytes(adxl_i2c, 0x32, accel_raws, 6)) return false;
  
  int16 xAccel = (accel_raws[1] << 8) | accel_raws[0];
  int16 yAccel = (accel_raws[3] << 8) | accel_raws[2];
  int16 zAccel = (accel_raws[5] << 8) | accel_raws[4];
  
  bind_stream(adxl, xAccel, ADXL_MEASURE_X);
  bind_stream(adxl, yAccel, ADXL_MEASURE_Y);
  bind_stream(adxl, zAccel, ADXL_MEASURE_Z);
  
  sensor_log_outputs(adxl, adxl_i2c -> log, NULL);
  sensor_process_triggers(adxl);
  return true;
}
