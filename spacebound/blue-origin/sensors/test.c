#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "ds32.h"
#include "test.h"

#include "../system/color.h"
#include "../system/file.h"
#include "../system/gpio.h"
#include "../system/i2c.h"

void free_test(Sensor * sensor);
bool read_test(i2c_device * test_i2c);

Sensor * init_test(Sensor * test) {
  
  test -> name = "TEST_SENSOR";
  test -> free = free_test;
  
  test -> i2c = create_i2c_device(test, read_test);
  test -> i2c -> log = safe_open("logs/test.log", "a");
  
  printf("logged in logs/test.log\n");
  printf("A virtual test sensor\n\n");
  
  
  /* set up output data streams */
  
  fprintf(test -> i2c -> log, RED "\n\nTEST_SENSOR\nStart time %s\n", formatted_time);
  fprintf(test -> i2c -> log,          "Real Time [%s]\tOS Time [s]", time_unit);
  
  for (int i = 0; i < test -> data_streams; i++) {
    
    Output * output = &test -> outputs[i];
    
    fprintf(test -> i2c -> log, "\t%s [%s]", output -> name, output -> unit);
  }
  
  fprintf(test -> i2c -> log, "\n" RESET);
  return test;
}

bool read_test(i2c_device * test_i2c) {
  
  Sensor * test = test_i2c -> sensor;
  
  
  Output * output;    // this sensor is virtual, so the steps are one and the same
  
  output = &test -> outputs[TEST_MEASURE_ZERO];
  output -> measure = series_compute(output -> series, 0);
  
  output = &test -> outputs[TEST_MEASURE_IDENTITY];
  output -> measure = series_compute(output -> series, output -> measure);
  
  output = &test -> outputs[TEST_MEASURE_SINE];
  output -> measure = series_compute(output -> series, sin(time_passed() / 10));
  
  output = &test -> outputs[TEST_MEASURE_COSINE];
  output -> measure = series_compute(output -> series, cos(time_passed() / 10));
  
  output = &test -> outputs[TEST_MEASURE_RANDOM];
  output -> measure = series_compute(output -> series, rand());
  
  
  /* process output streams */
  
  if (test -> print)
    sensor_print_to_console(test);
  
  sensor_log_outputs(test, test_i2c -> log);
  sensor_process_triggers(test);
  return true;
}

void free_test(Sensor * ds32) {
  // nothing needs to happen
}
