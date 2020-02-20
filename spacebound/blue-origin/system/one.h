#pragma once

#include "../include/headers.h"

typedef struct one_device {
  
  Sensor * sensor;
  
  char * path;             // where to get data
  FILE * log;              // log file
  
  int interval;            // time span between schedules in ms
  int hertz_numerator;     // reads per second
  int hertz_denominator;   // engenders fractional frequency through deferrals
  int count;               // counts since last read
  
  one_reader read;
  
} one_device;
