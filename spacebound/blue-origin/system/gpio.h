#pragma once

#include "../include/headers.h"

typedef struct Charge {
  
  char gpio;     // the broadcom number of the pin
  int  delay;    // used for pulsing
  
} Charge;

typedef struct Pin {
  
  int broadcom;
  
  union {
    bool hot;
    int duty;
  };
  
  int  ms_until_pulse_completes;    // ms remaining until pulse completes
  bool pulse_final_state;           // state needed at the end of pulse
  bool pulses;                      // whether pin may pulse
  
} Pin;
