
#include "../include/program.h"

local Pin pins[28];                                     // all gpio pins we have control over

void pin_set(char gpio, bool hot) {                     // set a pin high or low
  
  if (pins[gpio].hot == hot && pins[gpio].ever_used)    // don't actuate when unnecessary
    return;
  
  gpioWrite(gpio, (int) hot);
  
  if (hot) printf(YELLOW "%7.3f%s    set pin % 2d pos\n" RESET, time_passed(), time_unit, gpio);
  else     printf(YELLOW "%7.3f%s    set pin % 2d neg\n" RESET, time_passed(), time_unit, gpio);
  
  if (hot) fprintf(schedule -> control_log, "%f%s\twire\t%d\thot\n" , time_passed(), time_unit, gpio);
  else     fprintf(schedule -> control_log, "%f%s\twire\t%d\tcold\n", time_passed(), time_unit, gpio);
  
  pins[gpio].hot       = hot;
  pins[gpio].ever_used = true;
}

void pin_queue(PinChange * change, bool hot) {          // induces a pin change, possibly delaying
  
  if (!change -> delay) {
    pin_set(change -> gpio, hot);
    return;
  }
  
  pins[change -> gpio].queued_delay = change -> delay;
  pins[change -> gpio].queued_hot   = hot;
}

void process_pin_queue(int duration) {                 // process any delayed actuations
  
  for (int i = 0; i < 28; i++) {
    
    Pin * pin = &pins[i];
    
    when (pin -> queued_delay);                         // pin isn't queued
    
    pin -> queued_delay -= duration;
    
    when (pin -> queued_delay <= 0);
    
    pin_set(i, pin -> queued_hot);
    pin -> queued_delay = 0;
  }
}

char * recover_from_crash() {                           // when in gdb, prevent pigpio daemon from corrupting
  gpioTerminate();
  return "success";
}
