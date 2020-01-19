#pragma once

/**
 * @Attribute #1 Raw sensor readings are converted to sensor "measures"
 * through output-specific "series". A series is a linked list of computational
 * steps that must be performed to take each unit to the next. Some of these 
 * are known by the Exp compiler, such as the conversion from Celcius to 
 * Fahrenheit. Such known steps are known as "conversions". Others, such as 
 * leaps from one unit type like Voltage to another like Pressure, require 
 * a user-provided step known as a "calibration". Thus, each data stream is 
 * organized as follows:
 *   
 *   Raw  -->  Conversion & Calibration Series  -->  Measure
 *   
 *   Measure  =  (Conversion | Calibration)+  on  ( Raw )
 * 
 * @Invariant #3 Numerics always have a "units" name, even generic ints and
 * floats, which use 'i' and 'f', respectively.
 * 
 * @Invariant #4 Numerics, when used in a series, must be in float form.
 * 
 **/

#include "../include/headers.h"

typedef struct Numeric {
  
  union {
    int   integer;     // the numerical value 
    float decimal;     // -------------------
  };
  
  char units[8];       // the units code
  bool is_decimal;     // representation
  
} Numeric;

typedef struct Calibration {
  
  char * curve;        // the shape of the curve used to compute values
  List * constants;    // the constants defining the curve
  char * target;       // the data stream the calibration acts on
  char * unit_from;    // the unit from which to convert
  char * unit_to;      // the unit to which to convert
  bool   used;         // whether a series is using this
  
} Calibration;

typedef struct SeriesElement {
  
  bool universal;
  
  union {
    Calibration * calibration;   // if sensor specific
    Conversion    conversion;    // if universal conversion (Ex: Celcius to Fahrenheit)
  };
  
} SeriesElement;
