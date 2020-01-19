
#include "../include/program.h"

local Hashmap * states;    // maps state names to bools

Hashmap * state_delays;    // maps 

void init_states() {
  states       = hashmap_create(hash_string, compare_strings, free, NULL, 16);
  state_delays = hashmap_create(hash_string, compare_strings, NULL, free, 16);
}

void drop_states() {
  hashmap_delete(states);
  hashmap_delete(state_delays);
  state_delays = states = NULL;
}

void add_state(char * state, bool entered) {
  hashmap_add(states, state, (void *) entered);
}

bool state_exists(char * state) {
  return hashmap_exists(states, state);
}

void enter_state(char * state) {
  
  float event_time = time_passed();
  
  printf(CYAN "%f%s    enter %s\n" RESET, event_time, time_unit, state);
  hashmap_update(states, state, (void *) true);
  
  fprintf(schedule -> control_log, "%f%s\tstate\t%s\tenter\n", event_time, time_unit, state);
}

void leave_state(char * state) {
  
  float event_time = time_passed();
  
  printf(CYAN "%f%s    leave %s\n" RESET, event_time, time_unit, state);
  hashmap_update(states, state, (void *) false);
  
  fprintf(schedule -> control_log, "%f%s\tstate\t%s\tleave\n", event_time, time_unit, state);
}

void enter(Transition * trans) {
  
  if (!trans -> delay) {
    enter_state(trans -> state);
    return;
  }
  
  StateDelay * state_delay = hashmap_get(state_delays, trans -> state);
  state_delay -> ms_remaining = trans -> delay;
  state_delay -> entering = true;
}

void leave(Transition * trans) {
  
  if (!trans -> delay) {
    leave_state(trans -> state);
    return;
  }
  
  StateDelay * state_delay = hashmap_get(state_delays, trans -> state);
  state_delay -> ms_remaining = trans -> delay;
  state_delay -> entering = false;
}

bool state_get(char * state) {
  return (bool) hashmap_get(states, state);
}

void state_inform_delays(char * state) {
  // make it known that this state can delay
  
  if (!hashmap_exists(state_delays, state)) {
    
    StateDelay * state_delay = calloc(1, sizeof(*state_delay));
    
    state_delay -> state = state;
    
    hashmap_add(state_delays, state, state_delay);
  }
}

Transition * transition_create(char * state, int delay) {
  
  Transition * transition = calloc(1, sizeof(*transition));
  
  transition -> state = state;
  transition -> delay = delay;
  
  return transition;
}

void print_all_states() {
  
  printf(GREEN "States\n    " RESET);
  
  for (iterate(states -> keys, char *, name)) {
    if ((bool) hashmap_get(states, name)) printf(YELLOW "+" RESET "%s ", name);
    else                                  printf(YELLOW "-" RESET "%s ", name);
  }
  
  printf("\n\n");
}
