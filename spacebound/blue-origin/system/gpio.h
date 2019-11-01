#pragma once

#include <stdbool.h>

#include "../sensors/sensor.h"

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

void init_pins();
void pin_set(char broadcom, bool hot);
void pin_set_hot (void * nil, char * vbroadcom);    // selector commands
void pin_set_cold(void * nil, char * vbroadcom);    // -----------------
void pin_inform_delays(char broadcom);
void pin_pulse(char broadcom, int ms, bool hot);
void fire(Charge * charge, bool hot);
