#ifndef HEADER_GAURD_CLOCK
#define HEADER_GAURD_CLOCK

#include <time.h>


void real_sleep(time_t seconds);    // sleeps a real number of nanoseconds

void real_milli_sleep(long ms);   // sleeps a real number of mil
void real_nano_sleep(long ns);    // sleeps a real number of nanoseconds

long real_time_diff(struct timespec * past);    // difference in nanoseconds

#endif
