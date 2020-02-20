#pragma once

#include "../include/headers.h"

typedef struct Schedule {
  
  List * i2c_devices;           // list of all i2c device pointers
  List * one_devices;           // list of all 1-wire device pointers
  
  FILE * i2c_error_log;         // bus error logs
  FILE * one_error_log;         // --------------
  FILE * control_log;           // transition/charge log (i2c bound for now)
  
  i2c_device * last_i2c_dev;    // last i2c sensor measured
  one_device * last_one_dev;    // last 1-wire sensor measured
  
  long i2c_interval;            // scheduler spacing for the i2c protocol
  long one_interval;            // scheduler spacing for the 1-wire protocol
  long i2c_cycle;               // relative units for i2c interval lengths
  long one_cycle;               // relative units for 1-wire interval lengths
  
  bool i2c_active;              // whether experiment uses i2c
  bool one_active;              // whether experiment uses 1-wire
  
  bool print_sensors;           // whether any sensors print to the console
  
  int   interrupts;             // interrupts since schedule creation
  float interrupt_interval;     // time between each interrupt
  
  Thread * i2c_thread;          // i2c thread
  Thread * one_thread;          // 1-wire thread (deprioritized)
  bool term_signal;             // when set to true, schedule terminates
  
} Schedule;

extern Schedule * schedule;
