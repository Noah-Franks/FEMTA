#pragma once

#include "../include/headers.h"

typedef struct i2c_device {    // mechanism for i2c communication
  
  Sensor * sensor;             // the sensor bound to this device
  
  FILE * log;                  // the log file
  
  uint8 address;               // the address on bus 0x03-0x7F
  
  int interval;                // time span between reads in ms
  int hertz_numerator;         // schedule frequency in hertz
  int hertz_denominator;       // engenders fractional frequency through deferrals
  int count;                   // counts since last read
  
  bool reading;                // when true, scheduler knows to re-read
  
  int handle;                  // pigpio handle
  
  i2c_reader read;             // how to read from the bus
  
} i2c_device;

#define FAKE_ADDRESS 0x00      // reserved address for virtual sensors
