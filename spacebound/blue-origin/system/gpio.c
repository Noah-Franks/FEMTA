
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>

#include "gpio.h"
#include "color.h"

#include "../sensors/sensor.h"
#include "../structures/list.h"

Pin pins[28];

void init_pins() {
  
  schedule -> pulse_pins = list_create();
  
  for (int i = 0; i < 28; i++) {
    pins[i].broadcom                 = i;
    pins[i].hot                      = -1;
    pins[i].duty                     = -1;
    pins[i].pulses                   = false;
    pins[i].ms_until_pulse_completes = 0;
  }
  
  gpioWrite(23, 0);
  gpioWrite(24, 0);
  gpioWrite(27, 0);
}

void pin_inform_delays(char broadcom) {
  // let system know this pin pulses
  
  if (!pins[broadcom].pulses)
    list_insert(schedule -> pulse_pins, &pins[broadcom]);    // first time
  
  pins[broadcom].pulses = true;
}

void pin_set(char broadcom, bool hot) {
  printf(YELLOW "DEBUG: %d set %d\n" RESET, broadcom, hot);
  if (pins[broadcom].hot != hot) {
    pins[broadcom].hot = hot;
    gpioWrite(broadcom, (int) hot);    
  }
}

void pin_set_hot(void * nil, char * vbroadcom) {
  
  char broadcom = atoi((char *) vbroadcom + 1);
  
  printf("Set %d hot\n", broadcom);
  pin_set(broadcom, true);
}

void pin_set_cold(void * nil, char * vbroadcom) {
  
  char broadcom = atoi((char *) vbroadcom + 1);
  
  printf("Set %d cold\n", broadcom);
  pin_set(broadcom, false);
}

void fire(Charge * charge, bool hot) {
  // fires a charge, setting up any pulsing

  if (!charge -> delay) {
    pin_set(charge -> gpio, hot);
    return;
  }
  
  pins[charge -> gpio].ms_until_pulse_completes = charge -> delay;
  pins[charge -> gpio].pulse_final_state = hot;
}
