
#include "../include/program.h"

local void sensor_close(void *);
local void sensor_do_nothing(Sensor * nil) { return; }

char     * time_unit;
long       time_start_os;     // system start from the OS's perspective
List     * active_sensors;    // every active sensor on craft
List     * all_target_maps;   // all maps associating a target to its channels
Hashmap  * all_sensors;       // every sensor that could be specified

Sensor * sensor_create(char * code_name, int address, Hashmap * targets, int bus) {
  
  Sensor * proto = calloc(1, sizeof(*proto));    // Uninitialized sensors are called proto (by Invariant 0)
  
  proto -> code_name = code_name;
  proto -> address   = address;
  proto -> targets   = targets;
  proto -> bus       = bus;
  
  proto -> teardown  = sensor_do_nothing;
  
  // Check to see if the sensor produces data (possible by Invariant 1)
  if (targets && targets -> elements) {
    proto -> data_streams = targets -> elements;
    proto -> outputs      = calloc(targets -> elements, sizeof(*proto -> outputs));
    
    for (int stream = 0; stream < proto -> data_streams; stream++) {
      proto -> outputs[stream].regressive = 1.0f;
      proto -> outputs[stream].enabled    = true;
    }
    
    // assign a default name to each output channel
    for (iterate(targets -> keys, char *, name)) {
      proto -> outputs[(int) hashmap_get(targets, name)].name      = name;
      proto -> outputs[(int) hashmap_get(targets, name)].nice_name = name;    // default nice name
    }
    
    proto -> aliases = hashmap_create(hash_string, compare_strings, free, free, proto -> data_streams);
  }
  
  return proto;
}

void sensor_delete(void * vsensor) {
  
  Sensor * sensor = vsensor;
  
  for (int stream = 0; stream < sensor -> data_streams; stream++) {
    
    if (sensor -> outputs[stream].series)
      list_delete(sensor -> outputs[stream].series);
    
    if (sensor -> outputs[stream].triggers)
      list_delete(sensor -> outputs[stream].triggers);
    
    blank(sensor -> outputs[stream].unit);
  }
  
  if (sensor -> aliases)
    hashmap_delete(sensor -> aliases);
  
  blank(sensor -> outputs);
  free(sensor);
}

void trigger_delete(void * vtrigger) {
  
  Trigger * trigger = vtrigger;
  
  if (!trigger -> reverses) {
    
    trigger -> wires_low    = list_delete(trigger -> wires_low   );
    trigger -> wires_high   = list_delete(trigger -> wires_high  );
    trigger -> enter_set    = list_delete(trigger -> enter_set   );
    trigger -> leave_set    = list_delete(trigger -> leave_set   );
    trigger -> precondition = list_delete(trigger -> precondition);
    
    blank(trigger -> threshold_as_specified);
    blank(trigger -> id);
  }
  
  free(trigger);
}

void init_sensors() {
  
  sprintf(human_time, "[Clock not present! Defaulted to OS time!]");    // overwritten by clock
  ds32_time_unit = "s";                                                 // --------------------
  time_unit = "s";                                                      // TODO: remove
  
  all_sensors = hashmap_create(hash_string, compare_strings, NULL, sensor_delete, 16);
  
  Hashmap * adxl_tar = hashmap_create(hash_string, compare_strings, NULL, NULL, 3);
  Hashmap * ad15_tar = hashmap_create(hash_string, compare_strings, NULL, NULL, 4);
  Hashmap * arm6_tar = hashmap_create(hash_string, compare_strings, NULL, NULL, 3);
  Hashmap * ds32_tar = hashmap_create(hash_string, compare_strings, NULL, NULL, 2);
  Hashmap * ds18_tar = hashmap_create(hash_string, compare_strings, NULL, NULL, 1);
  Hashmap * mcp9_tar = hashmap_create(hash_string, compare_strings, NULL, NULL, 1);
  Hashmap * test_tar = hashmap_create(hash_string, compare_strings, NULL, NULL, 5);
  
  hashmap_add(ds32_tar, "Time"       , (void *) (int) DS32_MEASURE_TIME       );
  hashmap_add(ds32_tar, "Temperature", (void *) (int) DS32_MEASURE_TEMPERATURE);
  
  hashmap_add(ds18_tar, "Temperature", (void *) (int) DS18_MEASURE_TEMPERATURE);
  
  hashmap_add(mcp9_tar, "Temperature", (void *) (int) MCP9_MEASURE_TEMPERATURE);
  
  hashmap_add(test_tar, "zero"       , (void *) (int) TEST_MEASURE_ZERO       );
  hashmap_add(test_tar, "identity"   , (void *) (int) TEST_MEASURE_IDENTITY   );
  hashmap_add(test_tar, "sine"       , (void *) (int) TEST_MEASURE_SINE       );
  hashmap_add(test_tar, "cosine"     , (void *) (int) TEST_MEASURE_COSINE     );
  hashmap_add(test_tar, "random"     , (void *) (int) TEST_MEASURE_RANDOM     );
  
  hashmap_add(adxl_tar, "X"          , (void *) (int) ADXL_MEASURE_X          );
  hashmap_add(adxl_tar, "Y"          , (void *) (int) ADXL_MEASURE_Y          );
  hashmap_add(adxl_tar, "Z"          , (void *) (int) ADXL_MEASURE_Z          );
  
  hashmap_add(arm6_tar, "Load"       , (void *) (int) ARM6_MEASURE_LOAD       );
  hashmap_add(arm6_tar, "Memory"     , (void *) (int) ARM6_MEASURE_MEMORY     );
  hashmap_add(arm6_tar, "Temperature", (void *) (int) ARM6_MEASURE_TEMPERATURE);
  
  hashmap_add(ad15_tar, "A0"         , (void *) (int) AD15_MEASURE_A0         );
  hashmap_add(ad15_tar, "A1"         , (void *) (int) AD15_MEASURE_A1         );
  hashmap_add(ad15_tar, "A2"         , (void *) (int) AD15_MEASURE_A2         );
  hashmap_add(ad15_tar, "A3"         , (void *) (int) AD15_MEASURE_A3         );
  
  hashmap_add(all_sensors, "adxl"    , sensor_create("adxl", ADXL_ADDRESS, adxl_tar, I2C_BUS));
  hashmap_add(all_sensors, "ds32"    , sensor_create("ds32", DS32_ADDRESS, ds32_tar, I2C_BUS));
  hashmap_add(all_sensors, "ds18"    , sensor_create("ds18",            0, ds18_tar, ONE_BUS));
  hashmap_add(all_sensors, "mcp9"    , sensor_create("mcp9", MCP9_ADDRESS, mcp9_tar, I2C_BUS));
  hashmap_add(all_sensors, "arm6"    , sensor_create("arm6", FAKE_ADDRESS, arm6_tar, I2C_BUS));
  hashmap_add(all_sensors, "test"    , sensor_create("test", FAKE_ADDRESS, test_tar, I2C_BUS));
  hashmap_add(all_sensors, "ad15_gnd", sensor_create("ad15_gnd", AD15_GND, ad15_tar, I2C_BUS));
  hashmap_add(all_sensors, "ad15_vdd", sensor_create("ad15_vdd", AD15_VDD, ad15_tar, I2C_BUS));
  hashmap_add(all_sensors, "ad15_sda", sensor_create("ad15_sda", AD15_SDA, ad15_tar, I2C_BUS));
  hashmap_add(all_sensors, "ad15_scl", sensor_create("ad15_scl", AD15_SCL, ad15_tar, I2C_BUS));
  
  all_target_maps = list_from(7, adxl_tar, ad15_tar, arm6_tar, ds32_tar, ds18_tar, mcp9_tar, test_tar);
  all_target_maps -> value_free = vhashmap_delete;
  
  active_sensors = list_that_frees(sensor_close);
}

void drop_sensors() {
  all_target_maps = list_delete(all_target_maps);
  active_sensors  = list_delete(active_sensors);
  all_sensors     = hashmap_delete(all_sensors);
}

void sensor_close(void * vsensor) {
  // closes an active sensor
  
  Sensor * sensor = vsensor;
  
  printf(YELLOW "Removing sensor %s\n" RESET, sensor -> name);
  
  $(sensor, teardown);    // give opportunity for sensor-specific freeing
  
  if      (sensor -> bus == I2C_BUS) i2c_close(sensor -> i2c);
  else if (sensor -> bus == ONE_BUS) one_close(sensor -> one);
}

void flip_print(void * nil, char * raw_text) {
  // toggle sensor printing for the sensor specified
  
  if (strlen(raw_text) < 3) return;  // no such sensor
  
  raw_text[strlen(raw_text) - 1] = '\0';
  
  for (iterate(active_sensors, Sensor *, sensor))
    if (!strcmp(sensor -> code_name, raw_text + 2))
      sensor -> print = !sensor -> print;
}

void bind_stream(Sensor * sensor, float reading, int stream) {
  // processes a reading
  
  Output * output = &sensor -> outputs[stream];
  
  output -> measure = series_compute(output -> series, reading);
  
  // TODO: smooth here instead
}

void sensor_log_header(Sensor * sensor, char * color) {
  // the default log header for when an experiment starts
  // the file is assumed to already be open
  
  FILE * file = NULL;
  
  if (sensor -> bus == I2C_BUS) file = sensor -> i2c -> log;
  else                          file = sensor -> one -> log;
  
  fprintf(file, "\n\n%s%s\nStart time %s\n", color, sensor -> name, human_time);
  fprintf(file, "Real Time [%s]\tOS Time [s]", ds32_time_unit);
  
  for (int i = 0; i < sensor -> data_streams; i++) {
    
    Output * output = &sensor -> outputs[i];
    
    fprintf(file, "\t%s [%s]", output -> nice_name, output -> unit);
  }
  
  fprintf(file, "\n" RESET);
}

void sensor_log_outputs(Sensor * sensor, FILE * file, char * addendum) {
  // the format for every sensor log.
  // the log file is assumed to be open.
  
  fprintf(file, "%.5f\t%.4f", time_passed(), 0);
  
  for (int i = 0; i < sensor -> data_streams; i++)
    fprintf(file, "\t%.4f", sensor -> outputs[i].measure);
  
  if (addendum) fprintf(file, "\t%s\n", addendum);
  else          fprintf(file,     "\n"          );
}

void consider_printing_sensors(int duration) {
  local int print_delay = 0;
  
  if (!schedule -> print_sensors || (print_delay -= duration) > 0)
    return;
  
  print_delay = console_print_interval;
  
  printf(GREY "% 7.3lf%s  ", time_passed(), time_unit);
  
  for (iterate(active_sensors, Sensor *, sensor)) {
    
    when (sensor -> print);
    
    for (int stream = 0; stream < sensor -> data_streams; stream++) {
      
      Output * output = &sensor -> outputs[stream];
      
      when (output -> print);
      
      printf("%s% *.*f%s  ", output -> print_code,
             output -> print_places + strlen(output -> unit) + 4,
             output -> print_places,
             output -> measure, output -> unit);
    }
  }
  
  printf("\n" RESET);
}

void sensor_process_triggers(Sensor * sensor) {
  
  for (int stream = 0; stream < sensor -> data_streams; stream++) {
    
    Output * output = &sensor -> outputs[stream];
    
    when (output -> enabled);
    
    // apply smoothing before considering triggers
    output -> smoothed = 
      (       output -> regressive) * (output -> measure ) + 
      (1.0f - output -> regressive) * (output -> smoothed);
    
    float measure = output -> smoothed;    // by default, smoothing equals measure
    
    for (iterate(output -> triggers, Trigger *, trigger)) {
      
      bool precondition_met = true;
      for (iterate(trigger -> precondition, State *, state)) {
        if (!state -> entered) {
          precondition_met = false;
          break;
        }
      }
      
      if (!precondition_met                      ) continue;                       // not in all states required
      if (trigger -> singular && trigger -> fired) continue;                       // never fire singulars twice
      
      float threshold = trigger -> threshold;                                      // same units (by Invariant 2)
      
      if ( trigger -> less && measure > threshold) continue;                       // means condition is not true
      if (!trigger -> less && measure < threshold) continue;                       // ---------------------------
      
      for (iterate(trigger -> wires_low , PinChange *, change)) pin_queue(change, false);
      for (iterate(trigger -> wires_high, PinChange *, change)) pin_queue(change, true );
      
      for (iterate(trigger -> enter_set, StateChange *, change)) state_queue(change, true );
      for (iterate(trigger -> leave_set, StateChange *, change)) state_queue(change, false);
      
      trigger -> fired = true;
    }
  }
}
