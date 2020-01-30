
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
  
  float identity = test -> outputs[TEST_MEASURE_IDENTITY].measure;
  
  bind_stream(test,                       0, TEST_MEASURE_ZERO    );
  bind_stream(test,                identity, TEST_MEASURE_IDENTITY);
  bind_stream(test, sin(time_passed() / 10), TEST_MEASURE_SINE    );
  bind_stream(test, cos(time_passed() / 10), TEST_MEASURE_COSINE  );
  bind_stream(test,                  rand(), TEST_MEASURE_RANDOM  );
  
  sensor_log_outputs(test, test_i2c -> log, NULL);
  sensor_process_triggers(test);
  return true;
}
