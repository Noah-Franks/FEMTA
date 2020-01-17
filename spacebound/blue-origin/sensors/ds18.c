
#include "../include/program.h"

local bool read_ds18(one_device * ds18_one);

Sensor * init_ds18(Sensor * ds18, char * path) {
  
  ds18 -> name = "DS18B20";
  ds18 -> one = create_one_device(ds18, path, "logs/ds18.log", read_ds18);
  
  Output * output = &ds18 -> outputs[DS18_MEASURE_TEMPERATURE];
  
  output -> enabled = true;    // always enabled
  
  if (!output -> series) {
    output -> series = list_from(1, series_element_from_conversion(convert_identity));
    output -> unit   = strdup("C");
  }
  
  fprintf(ds18 -> one -> log, RED "\n\nDS18B20\n Start time %s\nTemp [%s]\n" RESET,
	  formatted_time, output -> unit);
  
  return ds18;
}

bool read_ds18(one_device * ds18_one) {
  
  Sensor * ds18 = ds18_one -> sensor;
  
  FILE * file = safe_open(ds18_one -> path, "r");
  
  if (!file) {
    printf(RED "Could not read %s for ds18: %s\n" RESET, ds18_one -> path, strerror(errno));
    return false;
  }
  
  char raw[128];
  
  int len = fread(raw, 1, 127, file);
  raw[len] = '\0';
  fclose(file);
  
  int temperature_code = atoi(strchr(strchr(raw, '=') + 1, '=') + 1);
  
  if (!temperature_code || temperature_code == 85000) {
    printf(RED "Could not read %s for ds18: error code %d\n" RESET, ds18_one -> path, temperature_code);
    return false;
  }
  
  float temperature = temperature_code / 1000.0f;
  
  
  // collected raw values, now to assign output streams
  
  Output * output = &ds18 -> outputs[DS18_MEASURE_TEMPERATURE];
  
  output -> measure = series_compute(output -> series, temperature);
  
  if (ds18 -> print)
    printf("ds18      %.5fs    %.5f%s\n", time_passed(), output -> measure, output -> unit);
  
  fprintf(ds18_one -> log, "%.5f\t%.5f\n", time_passed(), temperature);
  
  sensor_process_triggers(ds18);
  return true;
}
