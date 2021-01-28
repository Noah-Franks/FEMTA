
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>

#include "gpio.h"

void init_pins() {
  
  for (int i = 0; i < 40; i++) {
    pins[i].hot  = 0;
    pins[i].duty = 0;
  }
  
  gpioWrite(23, 0);
  gpioWrite(24, 0);
}

void pin_set(char broadcom, bool hot) {
  
  if (pins[broadcom].hot != hot) {
    pins[broadcom].hot = hot;
    gpioWrite(broadcom, (int) hot);
  }
  
  printf("DEBUG: %d set %d\n", broadcom, hot);
}
