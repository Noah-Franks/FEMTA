

#include "../include/program.h"

const float adxl_bias_x =  0.0371;
const float adxl_bias_y = -0.0010;
const float adxl_bias_z = -0.0861;

local bool read_adxl(i2c_device * adxl_i2c);

Sensor * init_adxl(Sensor * adxl) {
  // proto was passed in, but we shall forever call it 'adxl' (by Sensor Invariant 0)
  
  adxl -> name = "ADXL345";
  adxl -> i2c = create_i2c_device(adxl, read_adxl);
  printf("logged in logs/adxl.log\n");
  printf("An accelerometer\n\n");
  
  adxl -> i2c -> log = safe_open("logs/adxl.log", "a");
  
  fprintf(adxl -> i2c -> log, RED "\n\nADXL345\nStart time %s\nAccel x\tAccel y\tAccel z\n" RESET, formatted_time);
  
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
  
  uint8 accel_raws[6];
  
  if (!i2c_read_bytes(adxl_i2c, 0x32, accel_raws, 6)) return false;
  
  int16 xAccel = (accel_raws[1] << 8) | accel_raws[0];
  int16 yAccel = (accel_raws[3] << 8) | accel_raws[2];
  int16 zAccel = (accel_raws[5] << 8) | accel_raws[4];
  
  /*float 
  
  fprintf(ad15_i2c -> log, "%lf\t%.3f\t%.3f\t%.3f\n", experiment_duration, );
  
  fprintf(adxl_i2c -> log, "%.3f\t%.3f\t%.3f\n",
	  xAccel * 0.004 - adxl_bias_x,
	  yAccel * 0.004 - adxl_bias_y,
	  zAccel * 0.004 - adxl_bias_z);
  
  sensor_process_triggers(adxl); */
  return true;
}
