#pragma once

#include "../include/headers.h"

typedef struct i2c_device {
  
  Sensor * sensor;
  
  FILE * log;              // log file
  
  uint8 address;           // address on bus
  
  int interval;            // time span between reads in ms
  int hertz;               // schedule frequency in hertz
  int hertz_denominator;     // engenders fractional frequency through deferrals
  int bus_deferrals;       // how many bus deferrals since last read
  int count;               // counts since last read
  
  bool reading;            // when true, scheduler knows to re-read
  
  int handle;              // pigpio handle

  int total_reads;         // total times this sensor has been read
  
  i2c_reader read;
  
} i2c_device;
