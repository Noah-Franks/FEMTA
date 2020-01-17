#pragma once

#include "../include/headers.h"

typedef struct Numeric {
  
  union {
    int   integer;    // the numerical value 
    float decimal;    // -------------------
  };
  
  char units[8];      // the units code
  bool is_decimal;    // representation
  
} Numeric;

typedef struct Calibration {
  
  char * curve;        // the type of curve used to compute values
  List * constants;    // the constants defining the curve
  char * target;       // data stream calibration acts on
  char * unit_from;    // the unit from which to convert
  char * unit_to;      // the unit to which to convert
  
} Calibration;

typedef struct SeriesElement {
  
  bool universal;
  
  union {
    Calibration * calibration;   // if sensor specific
    Conversion    conversion;    // if universal conversion (Ex: Celcius to Fahrenheit)
  };
  
} SeriesElement;
