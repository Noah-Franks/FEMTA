
#include <time.h>

#include "clock.h"


void real_sleep(time_t seconds) {

  struct timespec time_period;
  time_period.tv_sec  = seconds;
  time_period.tv_nsec = 0;
  
  clock_nanosleep(CLOCK_REALTIME, 0, &time_period, NULL);
}

void real_nano_sleep(long ns) {

  struct timespec time_period;
  time_period.tv_sec  = 0;
  time_period.tv_nsec = ns;

  clock_nanosleep(CLOCK_REALTIME, 0, &time_period, NULL);
}

void real_milli_sleep(long ms) {
  real_nano_sleep(ms * 1E6);
}


long real_time_diff(struct timespec * past) {
  
  struct timespec now;

  clock_gettime(CLOCK_REALTIME, &now);

  return (now.tv_sec - past->tv_sec) * 1E9 + (now.tv_nsec - past->tv_nsec);
}
