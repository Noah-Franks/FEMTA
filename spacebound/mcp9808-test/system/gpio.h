#pragma once

#include <stdbool.h>

typedef struct Pin {

  union {
    bool hot;
    int duty;
  };
  
} Pin;

Pin pins[40];

void init_pins();
void pin_set(char broadcom, bool hot);
