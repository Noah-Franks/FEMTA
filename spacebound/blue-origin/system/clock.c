
#include "../include/program.h"

void nano_sleep(long ns) {                       // sleep a number of ns using the real-time patch
  
  struct timespec time_period = { 0, ns };
  
  clock_nanosleep(CLOCK_REALTIME, 0, &time_period, NULL);
}

void micro_sleep(long us) {                      // sleep a number of us using the real-time patch
  nano_sleep(us * 1000l);
}

long real_time_diff(struct timespec * past) {    // ns since point in the past
  
  struct timespec now;
  
  clock_gettime(CLOCK_REALTIME, &now);
  
  return (now.tv_sec - past->tv_sec) * 1000000000l + (now.tv_nsec - past->tv_nsec);
}
