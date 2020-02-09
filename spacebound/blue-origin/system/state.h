#pragma once

typedef struct State {          // a flowchart state
  
  char * name;                  // the state's name
  bool   entered;               // whether system is in the state
  
  int  queued_delay;            // ms remaining until state change
  bool queued_enter;            // whether to enter after delay
  
} State;

typedef struct StateChange {    // specifies a state transition
  
  State * state;                // the state to change
  int     delay;                // used for delayed state changes
  
} StateChange;
