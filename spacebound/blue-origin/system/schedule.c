
#include "../include/program.h"

Schedule * schedule;

float time_passed() {
  //return (schedule -> interrupts) * (schedule -> interrupt_interval);
  //return (float) real_time_diff(&time_start_os) / 1E9;
  return (float) (time(NULL) - time_start_os);
}

void build_schedule() {
  
  int i2c_cycle = -1;
  int one_cycle = -1;
  
  for (iterate(all_sensors -> all, Sensor *, proto)) {
    
    when (proto -> requested && proto -> hertz_numerator);        // make sure .e specifies this sensor
    
    int simplifier = gcd(proto -> hertz_numerator, proto -> hertz_denominator);
    
    proto -> hertz_numerator   /= simplifier;                     // simplify the rational
    proto -> hertz_denominator /= simplifier;                     // ---------------------
    
    if (proto -> bus == I2C_BUS)
      if (i2c_cycle == -1) i2c_cycle = proto -> hertz_numerator;
      else                 i2c_cycle = lcm(proto -> hertz_numerator, i2c_cycle);
    else if (proto -> bus == ONE_BUS)
      if (one_cycle == -1) one_cycle = proto -> hertz_numerator;
      else                 one_cycle = lcm(proto -> hertz_numerator, one_cycle);
  }
  
  schedule -> i2c_interval = 1000000000l / (schedule -> i2c_cycle = i2c_cycle);
  schedule -> one_interval = 1000000000l / (schedule -> one_cycle = one_cycle);
  
  schedule -> i2c_active = i2c_cycle > 0;
  schedule -> one_active = one_cycle > 0;
  
  if (schedule -> i2c_active) {
    
    int interval = schedule -> i2c_interval / 1000000;
    
    if (interval > 9) printf("Building I2C schedule using " GREEN "%dms" RESET " intervals\n", interval);
    else              print_error("I2C schedule requires likely unservicable %dms intervals\n", interval);
  }
  
  if (schedule -> one_active) {
    
    int interval = schedule -> one_interval / 1000000;
    
    if (interval > 9) printf("Building 1-wire schedule using " GREEN "%dms" RESET " intervals\n", interval);
    else              print_error("1-Wire schedule requires likely unservicable %dms intervals\n", interval);
  }
  
  
  Sensor * proto;    // name of uninitialized sensor (See Invariant 0 in Sensor.h)
  
  /* i2c sensors */
  
  // ds32
  proto = hashmap_get(all_sensors, "ds32");
  
  if (proto -> requested) {
    Sensor * ds32 = init_ds32(proto);
    list_insert(active_sensors         , ds32       );
    list_insert(schedule -> i2c_devices, ds32 -> i2c);    // first so we can get the time
  }
  
  // arm6
  proto = hashmap_get(all_sensors, "arm6");
  
  if (proto -> requested) {
    Sensor * arm6 = init_arm6(proto);
    list_insert(active_sensors         , arm6       );
    list_insert(schedule -> i2c_devices, arm6 -> i2c);
  }
  
  // adxl
  proto = hashmap_get(all_sensors, "adxl");
  
  if (proto -> requested) {
    Sensor * adxl = init_adxl(proto);
    list_insert(active_sensors         , adxl       );
    list_insert(schedule -> i2c_devices, adxl -> i2c);
  }
  
  // mcp9
  proto = hashmap_get(all_sensors, "mcp9");
  
  if (proto -> requested) {
    Sensor * mcp9 = init_mcp9(proto);
    list_insert(active_sensors         , mcp9       );
    list_insert(schedule -> i2c_devices, mcp9 -> i2c);
  }
  
  // test
  proto = hashmap_get(all_sensors, "test");
  
  if (proto -> requested) {
    Sensor * test = init_test(proto);
    list_insert(active_sensors         , test       );
    list_insert(schedule -> i2c_devices, test -> i2c);
  }
  
  // ad15
  Sensor * ad15[4] = {
    NULL, NULL, NULL, NULL,
  };
  
  char * code_names[4] = {
    "ad15_gnd", "ad15_vdd", "ad15_sda", "ad15_scl"
  };
  
  for (int sensor_index = 0; sensor_index < 4; sensor_index++) {
    
    proto = hashmap_get(all_sensors, code_names[sensor_index]);
    
    if (proto -> requested) {
      ad15[sensor_index] = init_ad15(proto, list_from(4, A0, A1, A2, A3));
      list_insert(active_sensors, ad15[sensor_index]);
    }
  }
  
  for (int channel = 0; channel < 4; channel++)                     // interleave the schedules
    for (int sensor_index = 0; sensor_index < 4; sensor_index++)    // ------------------------
      if (ad15[sensor_index])
        list_insert(schedule -> i2c_devices, ad15[sensor_index] -> i2c);
  
  
  /* 1-wire sensors */
  
  // ds18
  proto = hashmap_get(all_sensors, "ds18");
  
  if (proto -> requested) {
    //Sensor * ds18 = init_ds18(proto, "/sys/bus/w1/devices/28-000008e222e7/w1_slave");
    //Sensor * ds18 = init_ds18(proto, "/sys/bus/w1/devices/28-0115a6756cff/w1_slave");
    Sensor * ds18 = init_ds18(proto, "/sys/bus/w1/devices/28-000008e3f48b/w1_slave");
    //Sensor * ds18 = init_ds18(proto, "/sys/bus/w1/devices/28-0315a66ea4ff/w1_slave");
    list_insert(active_sensors,          ds18       );
    list_insert(schedule -> one_devices, ds18 -> one);
  }
  
  
  /* see if we can disable printing */
  
  bool some_sensor_prints = false;
  
  for (iterate(active_sensors, Sensor *, sensor))
    if (sensor -> print)
      some_sensor_prints = true;
  
  schedule -> print_sensors = some_sensor_prints && print_to_console;
}
