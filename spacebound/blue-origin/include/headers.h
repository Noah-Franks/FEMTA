#pragma once

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <stdarg.h>
#include <pigpio.h>
#include <sched.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <sys/prctl.h>


/* primatives */

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef signed char schar;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef pthread_t        Thread;
typedef pthread_mutex_t  Mutex;


// Lambda Calculus
typedef void (* lambda)(void *);


/* type definitions */

// ad15.h (sensors)
typedef struct AD15_Config AD15_Config;

// gpio.h (system)
typedef struct Charge Charge;
typedef struct Pin Pin;

// hashmap.h (structure)
typedef struct Hashmap Hashmap;
typedef struct HashmapElement HashmapElement;

// i2c.h (system)
typedef struct i2c_device i2c_device;

// list.h (structure)
typedef struct List List;
typedef struct ListNode ListNode;

// one.h (system)
typedef struct one_device one_device;

// sensor.h (sensors)
typedef struct Sensor Sensor;
typedef struct Output Output;
typedef struct Trigger Trigger;
typedef struct Schedule Schedule;

// state.h (system)
typedef struct Transition Transition;
typedef struct StateDelay StateDelay;

// units.h (math)
typedef struct Numeric Numeric;
typedef struct Calibration Calibration;
typedef struct SeriesElement SeriesElement;


/* function pointers */

// hashmap.h (structure)
typedef int  (* hash_function  )(void *, int upper_bound   );
typedef int  (* key_comparator )(void * first, void * other);
typedef void (* element_printer)(HashmapElement * element  );

// i2c.h (system)
typedef bool (* i2c_reader)(i2c_device * i2c);

// list.h (structure)
typedef void (* freer)();

// one.h (system)
typedef bool (* one_reader)(one_device * one);

// sensor.h (sensors)
typedef void (* sensor_free)(Sensor * sensor);

// units.h (math)
typedef float (* Conversion)(float value);

// user.h (system)
typedef void (* user_action)(void * arg, char * raw_text);


#include "../math/math.h"
#include "../math/units.h"
#include "../sensors/ad15.h"
#include "../sensors/adxl.h"
#include "../sensors/ds18.h"
#include "../sensors/ds32.h"
#include "../sensors/mcp9.h"
#include "../sensors/sensor.h"
#include "../sensors/test.h"
#include "../structures/hashmap.h"
#include "../structures/list.h"
#include "../system/color.h"
#include "../system/compiler.h"
#include "../system/error.h"
#include "../system/gpio.h"
#include "../system/i2c.h"
#include "../system/one.h"
#include "../system/state.h"
#include "../.origin/origin.h"
