#pragma once

/**
 * @Invariant #0 A sensor shall always be called "proto" until it is initialized.
 * Hence, all sensor initialization functions are "pass-through" since one may
 * never refer to an initialized sensor through a pointer called "proto".
 * Moreover, one never refers to a "ds32" until its "proto" form has been
 * initialized.
 * 
 * @Invariant #1 If a sensor produces data, structures must exist for triggers
 * 
 * @Invariant #2 After parsing completes, all triggers are in the same units that
 * output streams and log files use.
 * 
 * @Attribute #0 Sensors data production is organized into streams, which
 * are series of values over time on which triggers may act.
 */

#include "../include/headers.h"

#define I2C_BUS 0x1
#define ONE_BUS 0x2

typedef struct Trigger {
  
  char * id;                           // target (like A01)
  float  threshold;                    // threshold for condition
  List * precondition;                 // states the system must be in to consider this trigger
  
  List * wires_low;                    // wires to set low
  List * wires_high;                   // wires to set high
  List * enter_set;                    // sub-states to enter
  List * leave_set;                    // sub-states to leave
  
  bool less;                           // whether comparison is '<' or '>'
  bool fired;                          // whether trigger has ever fired
  bool singular;                       // whether trigger should only ever be fired once
  bool reverses;                       // whether trigger was source for its chiral opposite
  
  // parser only
  Numeric * threshold_as_specified;    // threshold as specified in E
  
} Trigger;

typedef struct Output {
  
  float  measure;         // measurement last taken
  float  smoothed;        // measurement after smoothing
  List * series;          // path of conversions required to get final measurement
  List * triggers;        // sensor triggers for this particular output stream
  char * name;            // name of the output for the log file - CHECK: used
  char * unit;            // final unit for logging and triggers
  char * nice_name;       // nicer name used for the log file and console
  float  regressive;      // smoothing constant
  bool   enabled;         // whether output is enabled
  bool   print;           // whether measures should be printed
  char * print_code;      // color to use while printing
  int    print_places;    // number of decimal places to use when printing
  
} Output;

typedef struct Sensor {
  
  char * name;              // component name
  char * code_name;         // abbreviated name
  bool   print;             // whether sensor prints
  
  union {
    i2c_device * i2c;       // i2c communications info
    one_device * one;       // 1-wire communications info
  };
  int bus;                  // which bus is used
  
  int data_streams;         // number of output data axes
  Output * outputs;         // everything this sensor produces
  Hashmap * targets;        // that which can be triggered (target str -> stream_index)
  Hashmap * aliases;        // alternative names for each channel
  
  sensor_free teardown;     // how to free sensor  
  
  int hertz_numerator;       // bus communication frequency in hertz
  int hertz_denominator;     // engenders fractional frequency through deferrals
  
  bool requested;            // whether sensor has actually been specified during parsing
  
  uint8 address;             // i2c address
  
  void * data;               // sensor-specific data
  
} Sensor;

extern char *     time_unit;
extern long       time_start_os;     // system start from the OS's perspective
extern List    *  active_sensors;    // every active sensor on craft
extern Hashmap *  all_sensors;       // every sensor that could be specified
