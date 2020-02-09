
#include "../include/program.h"

local State * states;                                   // all the states
local int     n_states;                                 // the number of states

void drop_states() {                                    // remove all states
  
  for (int i = 0; i < n_states; i++)
    blank(states[i].name);
  
  blank(states);
}

State * state_create(char * name, bool enter) {         // create and track a new state
  
  states = realloc(states, (n_states + 1) * sizeof(*states));
  
  memset(states + n_states * sizeof(*states), 0, sizeof(*states));
  
  states[n_states].name    = name;
  states[n_states].entered = enter;
  
  return &states[n_states++];
}

State * state_get(char * name) {                        // return a state, if it exists in O(n)
  
  for (int i = 0; i < n_states; i++)
    if (!strcmp(states[i].name, name))
      return &states[i];
  
  return NULL;
}

void state_set(State * state, bool enter) {             // set a state
  
  float event_time = time_passed();
  
  if (enter) printf(CYAN "%7.3f%s    enter %s\n" RESET, event_time, time_unit, state -> name);
  else       printf(CYAN "%7.3f%s    leave %s\n" RESET, event_time, time_unit, state -> name);
  
  if (enter) fprintf(schedule -> control_log, "%f%s\tstate\t%s\tenter\n", event_time, time_unit, state -> name);
  else       fprintf(schedule -> control_log, "%f%s\tstate\t%s\tleave\n", event_time, time_unit, state -> name);
  
  state -> entered = enter;
}

void state_queue(StateChange * change, bool enter) {    // queue a state change, if necessary
  
  if (!change -> delay) {
    state_set(change -> state, enter);
    return;
  }
  
  change -> state -> queued_delay = change -> delay;
  change -> state -> queued_enter = enter;
}

void process_state_queue(int duration) {                // process any delayed state changes
  
  for (int i = 0; i < n_states; i++) {
    
    State * state = &states[i];
    
    when (state -> queued_delay);
    
    state -> queued_delay -= duration;
    
    when (state -> queued_delay <= 0);
    
    state_set(state, state -> queued_enter);
    state -> queued_delay = 0;
  }
}

void print_all_states() {                               // nicely print all the states
  
  printf(GREEN "States\n    " RESET);
  
  for (int i = 0; i < n_states; i++)
    if (states[i].entered) printf(YELLOW "+" RESET "%s ", states[i].name);
    else                   printf(YELLOW "-" RESET "%s ", states[i].name);
  
  printf("\n");
}
