
#include "../include/program.h"

local bool read_test(i2c_device * test_i2c);

Sensor * init_test(Sensor * test) {
  
  test -> name = "TEST_SENSOR";
  test -> i2c = create_i2c_device(test, read_test);
  test -> i2c -> log = safe_open("logs/test.log", "a");
  
  printf("logged in logs/test.log\n");
  printf("A virtual test sensor\n\n");
  
  sensor_log_header(test, GRAY);
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
  
  sensor_log_outputs(test, test_i2c -> log);
  sensor_process_triggers(test);
  return true;
}
