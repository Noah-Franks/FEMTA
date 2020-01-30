#pragma once

typedef struct Transition {
  
  char * state;           // the broadcom number of the pin
  int    delay;           // used for delayed state changes
  
} Transition;

typedef struct StateDelay {

  char * state;           // the state whose transition is under this delay
  int    ms_remaining;    // remaining ms before state change
  bool   entering;        // whether state should be entered
  
} StateDelay;

extern Hashmap * state_delays;
