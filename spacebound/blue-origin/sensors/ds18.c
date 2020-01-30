
#include "../include/program.h"

local bool read_ds18(one_device * ds18_one);

Sensor * init_ds18(Sensor * ds18, char * path) {
  
  ds18 -> name = "DS18B20";
  ds18 -> one = create_one_device(ds18, path, "logs/ds18.log", read_ds18);
  
  sensor_log_header(ds18, RED);
  return ds18;
}

bool read_ds18(one_device * ds18_one) {
  
  Sensor * ds18 = ds18_one -> sensor;
  
  FILE * file = safe_open(ds18_one -> path, "r");    // a pseudo-file
  
  if (!file) {
    printf(RED "Could not read %s for ds18: %s\n" RESET, ds18_one -> path, strerror(errno));
    return false;
  }
  
  char raw[128];
  int len = fread(raw, 1, 127, file);
  raw[len] = '\0';
  fclose(file);
  
  int temperature = atoi(strchr(strchr(raw, '=') + 1, '=') + 1);    // temp in mC
  
  if (!temperature || temperature == 85000) {
    printf(RED "Could not read %s for ds18: error code %d\n" RESET, ds18_one -> path, temperature);
    return false;
  }
  
  bind_stream(ds18, temperature, DS18_MEASURE_TEMPERATURE);
  
  sensor_log_outputs(ds18, ds18_one -> log, NULL);
  sensor_process_triggers(ds18);
  return true;
}
