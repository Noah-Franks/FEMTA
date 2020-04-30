
#include "../include/program.h"

local bool read_arm6(i2c_device * arm6_i2c);

Sensor * init_arm6(Sensor * arm6) {
  
  arm6 -> name = "ARM6_SENSOR";
  arm6 -> i2c = create_i2c_device(arm6, read_arm6, "A Central Processing Unit");
  
  sensor_log_header(arm6, GRAY);
  return arm6;
}

bool read_arm6(i2c_device * arm6_i2c) {
  
  Sensor * arm6 = arm6_i2c -> sensor;
  
  /* ask the Linux Kernel for usage statistics */
  
  struct rusage usage;
  
  getrusage(RUSAGE_SELF, &usage);
  
  float cpu_usage = (usage.ru_stime.tv_sec + usage.ru_utime.tv_sec + 1.0) / time_passed();
  
  int ram_usage;      // kilo-bytes of RAM in use
  int temperature;    // temperature of the physical cpu in mC
  
  if (!scan_file("/proc/self/statm", "%d", &ram_usage))
    return false;
  
  if (!scan_file("/sys/class/thermal/thermal_zone0/temp", "%d", &temperature))
    return false;
  
  /* process output streams */
  
  bind_stream(arm6, cpu_usage  , ARM6_MEASURE_LOAD       );
  bind_stream(arm6, ram_usage  , ARM6_MEASURE_MEMORY     );
  bind_stream(arm6, temperature, ARM6_MEASURE_TEMPERATURE);
  
  sensor_log_outputs(arm6, arm6_i2c -> log, NULL);
  sensor_process_triggers(arm6);
  return true;
}
