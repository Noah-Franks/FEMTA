
#include "../include/program.h"

local void sensor_close(void *);
local void sensor_do_nothing(Sensor * nil) { return; }

Schedule * schedule;
char     * time_unit;
long       time_start_os;     // system start from the OS's perspective
List     * active_sensors;    // every active sensor on craft
List     * all_target_maps;   // all maps associating a target to its channels
Hashmap  * all_sensors;       // every sensor that could be specified
int        n_triggers;        // number of triggers


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
    for (iterate(targets -> keys, char *, name))
      proto -> outputs[(int) hashmap_get(targets, name)].name = name;
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
  
  blank(sensor -> outputs);
  free(sensor);
}

void init_sensors() {
  
  n_triggers = 0;
  
  sprintf(formatted_time, "[Clock not present! Defaulted to OS time!]");    // overwritten by clock
  time_unit = "s";                                                          // --------------------
  
  all_sensors = hashmap_create(hash_string, compare_strings, NULL, sensor_delete, 16);
  
  Hashmap * adxl_tar = hashmap_create(hash_string, compare_strings, NULL, NULL, 3);
  Hashmap * ad15_tar = hashmap_create(hash_string, compare_strings, NULL, NULL, 6);
  Hashmap * ds32_tar = hashmap_create(hash_string, compare_strings, NULL, NULL, 1);
  Hashmap * ds18_tar = hashmap_create(hash_string, compare_strings, NULL, NULL, 1);
  Hashmap * mcp9_tar = hashmap_create(hash_string, compare_strings, NULL, NULL, 1);
  Hashmap * test_tar = hashmap_create(hash_string, compare_strings, NULL, NULL, 5);
  
  hashmap_add(ds32_tar, "Time"       , (void *) (int) DS32_MEASURE_TIME       );
  hashmap_add(ds32_tar, "Temperature", (void *) (int) DS32_MEASURE_TEMPERATURE);
  
  hashmap_add(ds18_tar, "Temperature", (void *) (int) 0);
  
  hashmap_add(mcp9_tar, "Temperature", (void *) (int) MCP9_MEASURE_TEMPERATURE);
  
  hashmap_add(test_tar, "zero"    , (void *) (int) TEST_MEASURE_ZERO    );
  hashmap_add(test_tar, "identity", (void *) (int) TEST_MEASURE_IDENTITY);
  hashmap_add(test_tar, "sine"    , (void *) (int) TEST_MEASURE_SINE    );
  hashmap_add(test_tar, "cosine"  , (void *) (int) TEST_MEASURE_COSINE  );
  hashmap_add(test_tar, "random"  , (void *) (int) TEST_MEASURE_RANDOM  );
  
  hashmap_add(adxl_tar, "X", (void *) (int) 0);
  hashmap_add(adxl_tar, "Y", (void *) (int) 1);
  hashmap_add(adxl_tar, "Z", (void *) (int) 2);
  
  hashmap_add(ad15_tar, "A01", (void *) (int) 0);    // if this doesn't make sense to you,
  hashmap_add(ad15_tar, "A23", (void *) (int) 1);    // really think it through cause it's 
  hashmap_add(ad15_tar, "A0" , (void *) (int) AD15_MEASURE_A0);    // critical to the ad15's interface
  hashmap_add(ad15_tar, "A1" , (void *) (int) AD15_MEASURE_A1);    // 
  hashmap_add(ad15_tar, "A2" , (void *) (int) AD15_MEASURE_A2);    // 
  hashmap_add(ad15_tar, "A3" , (void *) (int) AD15_MEASURE_A3);    // 
  
  
  hashmap_add(all_sensors, "adxl"    , sensor_create("adxl", ADXL_ADDRESS, adxl_tar, I2C_BUS));
  hashmap_add(all_sensors, "ds32"    , sensor_create("ds32", DS32_ADDRESS, ds32_tar, I2C_BUS));
  hashmap_add(all_sensors, "ds18"    , sensor_create("ds18",            0, ds18_tar, ONE_BUS));
  hashmap_add(all_sensors, "mcp9"    , sensor_create("mcp9", MCP9_ADDRESS, mcp9_tar, I2C_BUS));
  hashmap_add(all_sensors, "test"    , sensor_create("test", TEST_ADDRESS, test_tar, I2C_BUS));
  hashmap_add(all_sensors, "ad15_gnd", sensor_create("ad15_gnd", AD15_GND, ad15_tar, I2C_BUS));
  hashmap_add(all_sensors, "ad15_vdd", sensor_create("ad15_vdd", AD15_VDD, ad15_tar, I2C_BUS));
  hashmap_add(all_sensors, "ad15_sda", sensor_create("ad15_sda", AD15_SDA, ad15_tar, I2C_BUS));
  hashmap_add(all_sensors, "ad15_scl", sensor_create("ad15_scl", AD15_SCL, ad15_tar, I2C_BUS));
  
  all_target_maps = list_from(6, adxl_tar, ad15_tar, ds32_tar, ds18_tar, mcp9_tar, test_tar);
  all_target_maps -> value_free = hashmap_delete;
}

void drop_sensors() {
  
  list_delete(active_sensors);
  active_sensors = NULL;
  
  hashmap_delete(all_sensors);
  all_sensors = NULL;
  
  list_delete(all_target_maps);
  all_target_maps = NULL;
}

void start_sensors() {
  
  int i2c_divisor = -1;
  int one_divisor = -1;
  
  for (iterate(all_sensors -> all, Sensor *, proto)) {
    
    if (!proto -> requested || !proto -> hertz) continue;
    
    int interval = 1000 / proto -> hertz;
    
    if (interval * proto -> hertz != 1000)
      exit_printing
        (ERROR_EXPERIMENTER, "\n1000ms is not cleanly divided by %s's %dHz", proto -> code_name, proto -> hertz);
    
    switch (proto -> bus) {
      
    case I2C_BUS:
      if (i2c_divisor == -1) i2c_divisor = interval;
      i2c_divisor = gcd(interval, i2c_divisor);
      break;
      
    case ONE_BUS:
      if (one_divisor == -1) one_divisor = interval;
      one_divisor = gcd(interval, one_divisor);
      break;
      
    default:
      printf(RED "Bus type %x not supported\n" RESET, proto -> bus);
      break;
    }
  }
  
  
  
  if (i2c_divisor != -1) {
    
    printf("\nBuilding I2C sensor schedule using " GREEN "%dms" RESET " intervals\n\n", i2c_divisor);
    
    if (i2c_divisor < 10)
      printf(RED "I2C schedule with divisor %dms is likely unserviceable.\n"
             "Please consider more harmonious frequencies from below\n" RESET
             "\t 1, 2, 4, 5, 10, 20, 25, 40, 50, 100, 125, 200, 250\n\n", i2c_divisor);
    
    schedule -> i2c_active = true;
  }
  
  if (one_divisor != -1) {
    
    printf("\nBuilding 1-wire sensor schedule using " GREEN "%dms" RESET " intervals\n\n", one_divisor);
    
    if (one_divisor < 10)
      printf(RED "1-Wire schedule with divisor %dms is likely unserviceable.\n"
             "Please consider more harmonious frequencies from below\n" RESET
             "\t 1, 2, 4, 5, 10, 20, 25, 40, 50, 100, 125, 200, 250\n\n", one_divisor);

    schedule -> one_active = true;
  }
  
  schedule -> i2c_interval = i2c_divisor * 1E6;
  schedule -> one_interval = one_divisor * 1E6;
  
  active_sensors = list_that_frees(sensor_close);
  
  
  
  Sensor * proto;    // name of uninitialized sensor (See Invariant 0)
  
  /* i2c sensors */
  
  // ds32
  proto = hashmap_get(all_sensors, "ds32");
  
  if (proto -> requested) {
    Sensor * ds32 = init_ds32(proto);
    list_insert(active_sensors,          ds32       );
    list_insert(schedule -> i2c_devices, ds32 -> i2c);    // first so we can get the time
  }
  
  
  // adxl
  proto = hashmap_get(all_sensors, "adxl");

  if (proto -> requested) {
    Sensor * adxl = init_adxl(proto);
    list_insert(active_sensors,          adxl       );
    list_insert(schedule -> i2c_devices, adxl -> i2c);
  }
  
  
  // mcp9
  proto = hashmap_get(all_sensors, "mcp9");
  
  if (proto -> requested) {
    Sensor * mcp9 = init_mcp9(proto);
    list_insert(active_sensors,          mcp9       );
    list_insert(schedule -> i2c_devices, mcp9 -> i2c);
  }
  
  
  // test
  proto = hashmap_get(all_sensors, "test");
  
  if (proto -> requested) {
    Sensor * test = init_test(proto);
    list_insert(active_sensors,          test       );
    list_insert(schedule -> i2c_devices, test -> i2c);
  }
  
  
  // ad15
  Sensor * ad15[4] = {
    NULL, NULL, NULL, NULL,
  };
  
  proto = hashmap_get(all_sensors, "ad15_gnd");
  
  if (proto -> requested) {
    ad15[0] = init_ad15(proto, "Single channels",
			list_from(4, A0, A1, A2, A3),
			list_from(4, "10kOhm", "+3.3V", "Thermister 2", "Thermister 1"));

    list_insert(active_sensors, ad15[0]);
  }
  
  proto = hashmap_get(all_sensors, "ad15_vdd");
  
  if (proto -> requested) {
    ad15[1] = init_ad15(proto, "Alcohol Pressure",
			list_from(4, A0, A1, A2, A3),
			list_from(4, "ground", "+5V", "Thermister 3", "Thermister 6"));
    
    list_insert(active_sensors, ad15[1]);
  }

  proto = hashmap_get(all_sensors, "ad15_sda");

  if (proto -> requested) {
    //ad15[2] = init_ad15(proto, "Alcohol Pressure", list_from(2, A01, A23), list_from(2, "Diff 01", "Diff 23"));
    ad15[2] = init_ad15(proto, "Ambient Air",
			list_from(4, A0, A1, A2, A3),
			list_from(4, "Thermister 5", "Thermister 4", "Thermister 7", "Thermister 8"));
    
    list_insert(active_sensors, ad15[2]);
  }

  proto = hashmap_get(all_sensors, "ad15_scl");

  if (proto -> requested) {
    ad15[3] = init_ad15(proto, "Differentials",
                        list_from(4, A0, A1, A2, A3),
                        list_from(4, "Thermister 9", "Thermister 10", "Thermister 11", "Thermister 12"));
    list_insert(active_sensors, ad15[3]);
  }
    
  for (int channel = 0; channel < 4; channel++)
    for (int sensor_index = 0; sensor_index < 4; sensor_index++)
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

float time_passed() {
  
  //return (schedule -> interrupts) * (schedule -> interrupt_interval);
  //return (float) real_time_diff(&time_start_os) / 1E9;
  return (float) (time(NULL) - time_start_os);
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

void sensor_log_header(Sensor * sensor, char * color) {
  // the default log header for when an experiment starts
  
  FILE * file = NULL;
  
  if (sensor -> bus == I2C_BUS) file = sensor -> i2c -> log;
  else                          file = sensor -> one -> log;
  
  fprintf(file, "\n\n%s%s\nStart time %s\n", color, sensor -> name, formatted_time);
  fprintf(file, "Real Time [%s]\tOS Time [s]", time_unit);
  
  for (int i = 0; i < sensor -> data_streams; i++) {
    
    Output * output = &sensor -> outputs[i];
    
    fprintf(file, "\t%s [%s]", output -> name, output -> unit);
  }
  
  fprintf(file, "\n" RESET);
}

void sensor_log_outputs(Sensor * sensor, FILE * file) {
  // the format for every sensor log.
  // the log file is assumed to be open.
  
  fprintf(file, "%.5f\t%.4f", time_passed(), 0);
  
  for (int i = 0; i < sensor -> data_streams; i++) {
    
    Output * output = &sensor -> outputs[i];
    
    fprintf(file, "\t%.4f", output -> measure);
  }
  
  fprintf(file, "\n");
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
      for (iterate(trigger -> precondition, char *, state)) {
        if (!state_get(state)) {
          precondition_met = false;
          break;
        }
      }
      
      if (!precondition_met                      ) continue;                       // not in all states required
      if (trigger -> singular && trigger -> fired) continue;                       // never fire singulars twice
      
      float threshold = trigger -> threshold;                                      // same units (by Invariant 2)
      
      if ( trigger -> less && measure > threshold) continue;                       // means condition is not true
      if (!trigger -> less && measure < threshold) continue;                       // ---------------------------
      
      for (iterate(trigger -> wires_low , Charge *, charge)) fire(charge, false);
      for (iterate(trigger -> wires_high, Charge *, charge)) fire(charge, true );
      
      for (iterate(trigger -> enter_set, Transition *, trans)) enter(trans);
      for (iterate(trigger -> leave_set, Transition *, trans)) leave(trans);
      
      trigger -> fired = true;
    }
  }
}
