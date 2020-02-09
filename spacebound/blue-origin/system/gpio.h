#pragma once

#include "../include/headers.h"

typedef struct PinChange {    // specifies a pin actuation
  
  char gpio;                  // the broadcom number of the pin to set
  int  delay;                 // full ms interval until next set
  
} PinChange;

typedef struct Pin {          // a gpio pin
  
  union {
    bool  hot;                // whether pin is high
    uint8 duty;               // the pin's PWM dutycycle
  };
  
  int  queued_delay;          // ms remaining until pin change
  bool queued_hot;            // whether to go hot once delay is over
  bool ever_used;             // whether pin has ever been actuated
  
} Pin;
