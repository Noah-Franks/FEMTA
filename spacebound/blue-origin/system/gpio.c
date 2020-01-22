
#include "../include/program.h"

local Pin pins[28];

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

void drop_pins() {
  list_delete(schedule -> pulse_pins);
  schedule -> pulse_pins = NULL;
}

void pin_inform_delays(char broadcom) {
  // let system know this pin pulses
  
  if (!pins[broadcom].pulses)
    list_insert(schedule -> pulse_pins, &pins[broadcom]);    // first time
  
  pins[broadcom].pulses = true;
}

void pin_set(char broadcom, bool hot) {
  
  //printf(YELLOW "DEBUG: %d set %d\n" RESET, broadcom, hot);
  
  if (hot) printf(YELLOW "%7.3f%s    set pin %d pos\n" RESET, time_passed(), time_unit, broadcom);
  else     printf(YELLOW "%7.3f%s    set pin %d neg\n" RESET, time_passed(), time_unit, broadcom);
  
  if (pins[broadcom].hot != hot) {
    pins[broadcom].hot = hot;
    gpioWrite(broadcom, (int) hot);
    
    if (hot) fprintf(schedule -> control_log, "%f%s\twire\t%d\thot\n", time_passed(), time_unit, broadcom);
    else     fprintf(schedule -> control_log, "%f%s\twire\t%d\tcold\n", time_passed(), time_unit, broadcom);
  }
}

void pin_set_hot(void * nil, char * vbroadcom) {
  
  char broadcom = atoi((char *) vbroadcom + 1);
  
  float event_time = time_passed();
  
  printf("%f%s     set %d hot\n" , event_time, time_unit, broadcom);
  fprintf(schedule -> control_log, "%f%s\twire\t%d\thot\n", event_time, time_unit, broadcom);
  
  pin_set(broadcom, true);
}

void pin_set_cold(void * nil, char * vbroadcom) {
  
  char broadcom = atoi((char *) vbroadcom + 1);
  
  float event_time = time_passed();
  
  printf("%f%s     set %d cold\n" , event_time, time_unit, broadcom);
  fprintf(schedule -> control_log, "%f%s\twire\t%d\tcold\n", event_time, time_unit, broadcom);
  
  pin_set(broadcom, false);
}

void fire(Charge * charge, bool hot) {
  // fires a charge, setting up any pulsing
  
  //printf("Fired charge %d %d\n", charge -> gpio, hot);
  
  if (!charge -> delay) {
    pin_set(charge -> gpio, hot);
    return;
  }
  
  pins[charge -> gpio].ms_until_pulse_completes = charge -> delay;
  pins[charge -> gpio].pulse_final_state = hot;
}
