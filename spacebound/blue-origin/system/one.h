#pragma once

#include "../include/headers.h"

typedef struct one_device {
  
  Sensor * sensor;
  
  char * path;             // where to get data
  FILE * log;              // log file
  
  int interval;            // time span between schedules in ms
  int hertz;               // reads per second
  int hertz_denominator;   // engenders fractional frequency through deferrals
  int bus_deferrals;       // how many bus deferrals since last read
  int count;               // counts since last read
  
  int total_reads;         // total times this sensor has been read
  
  one_reader read;
  
} one_device;
