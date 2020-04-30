#pragma once

/**
 * Sensor:
 *   Analog to digital converter
 *   Texas Instruments ADS1115
 *   Connected via I2C
 * 
 * Datasheet: 
 *   https://cdn-shop.adafruit.com/datasheets/ads1115.pdf
 * 
 * Author: 
 *   Noah Franks
 */

#include "../include/headers.h"

#define AD15_MEASURE_A0 0
#define AD15_MEASURE_A1 1
#define AD15_MEASURE_A2 2
#define AD15_MEASURE_A3 3

#define A01 0b000
#define A23 0b011
#define A0  0b100
#define A1  0b101
#define A2  0b110
#define A3  0b111

#define AD15_GND 0x48
#define AD15_VDD 0x49
#define AD15_SDA 0x4A
#define AD15_SCL 0x4B

typedef struct AD15_Config {
  // read pages 18-19 of datasheet for more informaiton;
  // some of this has a bit of nuance.
  
  union {
    uchar low_byte;           // ease of byte access
    struct {
      uchar COMP_QUE : 2;     // number of conversions needed to trigger comparator
      uchar COMP_LAT : 1;     // whether to latch the comparator
      uchar COMP_POL : 1;     // polarity of the comparator
      uchar COMP_MODE: 1;     // type of comparator to employ
      uchar DATA_RATE: 3;     // samples per second
    };
  };
  
  union {
    uchar high_byte;          // ease of byte access
    struct {
      uchar MODE: 1;          // high or low-power state
      uchar PGA : 3;          // programmable gain amplifier
      uchar MUX : 3;          // multiplexer setup
      uchar OS  : 1;          // sleep state
    };
  };
  
  List * modes;               // list of modes used to change config as needed
  ListNode * current_mode;    // the most recent mode configured configured
  int mode_cycle;             // where in the read cycle the current mode is
  
} AD15_Config;
