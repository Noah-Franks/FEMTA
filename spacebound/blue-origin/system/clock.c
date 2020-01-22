
#include "../include/program.h"

void real_sleep(time_t seconds) {
  
  struct timespec time_period;
  time_period.tv_sec  = seconds;
  time_period.tv_nsec = 0;
  
  clock_nanosleep(CLOCK_REALTIME, 0, &time_period, NULL);
}

void milli_sleep(long ms) {
  nano_sleep(ms * 1000000l);
}

void micro_sleep(long us) {
  nano_sleep(us * 1000l);
}

void nano_sleep(long ns) {

  struct timespec time_period;
  time_period.tv_sec  = 0;
  time_period.tv_nsec = ns;

  clock_nanosleep(CLOCK_REALTIME, 0, &time_period, NULL);
}

long real_time_diff(struct timespec * past) {
  
  struct timespec now;
  
  clock_gettime(CLOCK_REALTIME, &now);
  
  //return (long) (now.tv_sec - past->tv_sec) * 1000000000l + (long) (now.tv_nsec - past->tv_nsec);
  return (now.tv_sec - past->tv_sec) * 1E9 + (now.tv_nsec - past->tv_nsec);
}
