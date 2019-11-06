
#define _GNU_SOURCE

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <pigpio.h>
#include <sys/prctl.h>
#include <time.h>

#include "clock.h"
#include "color.h"
#include "one.h"

#include "../structures/list.h"
#include "../sensors/sensor.h"
#include "../system/file.h"
#include "../types/types.h"
#include "../types/thread-types.h"


void * one_main();

one_device * create_one_device(Sensor * sensor, char * path, char * log_path, one_reader read) {
  
  one_device * one = calloc(1, sizeof(*one));
  
  one -> sensor = sensor;
  one -> path   = path;
  one -> log    = safe_open(log_path, "a");
  one -> read   = read;
  
  if (!one -> log) {
    printf(RED "Critical: could not open %d's log file %s\n" RESET, sensor -> name, log_path);
    exit(1);
  }
  
  one -> hertz             = sensor -> hertz;
  one -> hertz_denominator = sensor -> hertz_denominator;
  
  if (one -> hertz)
    one -> interval = 1000 / (one -> hertz);
  
  if (!one -> hertz_denominator)
    one -> hertz_denominator = 1;
  
  
  // give a nice message to the user
  
  printf("Started " GREEN "%s " RESET "at " YELLOW "%d", sensor -> name, one -> hertz);
  if (one -> hertz_denominator) printf("/%d", one -> hertz_denominator);
  printf("Hz " RESET "via " BLUE "%s " RESET, path);
  if (sensor -> print) printf("with " MAGENTA "printing" RESET);
  printf("\nlogged in %s\n", log_path);
  
  return one;
}

void one_close(one_device * one) {
  free(one);
}

void init_one() {
  // initialize 1-wire data structures
  
  schedule -> one_devices = list_create();
  // need to set freeing for 1-wire sensors
  schedule -> one_thread  = malloc(sizeof(*schedule -> one_thread));
}

void start_one() {
  
  if (!schedule -> one_active) return;
  
  printf("\nStarting 1-wire schedule with " MAGENTA "%d " RESET "events\n", schedule -> one_devices -> size);
  
  // create 1-wire thread
  if (pthread_create(schedule -> one_thread, NULL, one_main, NULL)) {
    printf(RED "Could not start 1-wire thread\n" RESET);
    return;
  }
}

void terminate_one() {
  
  list_destroy(schedule -> one_devices);
  
  free(schedule -> one_thread);
  
  schedule -> one_devices = NULL;
  schedule -> one_thread  = NULL;
}

void * one_main() {
  
  prctl(PR_SET_NAME, "1-wire sched", NULL, NULL, NULL);
  
  // deprioritize all 1-wire communication
  struct sched_param priority = {0};
  
  sched_getparam(0, &priority);
  
  priority.sched_priority /= 2;
  
  sched_setparam(0, &priority);
  
  FILE * one_log = safe_open("logs/one.log", "a");
  fprintf(one_log, GRAY "Read duration [ns]\n" RESET);

  long last_read_duration = 0;    // tracks time taken to read 1-wire bus
  
  long one_interval = schedule -> one_interval;
  
  while (!schedule -> term_signal) {
    
    // get time before we perform the read
    struct timespec pre_read_time;
    clock_gettime(CLOCK_REALTIME, &pre_read_time);
    
    fprintf(one_log, "%ld\n", last_read_duration);
    
    
    // perform sensor reading
    
    for (iterate(schedule -> one_devices, one_device *, one)) {
      
      one -> count += one_interval / 1E6;
      
      if (one -> count == (one -> interval) * (one -> hertz_denominator)) {
        
        (one -> read)(one);
        
        one -> count = 0;
      }
    }
    
    
    // figure out how long to sleep
    
    long read_duration = real_time_diff(&pre_read_time);
    
    long time_remaining = one_interval - read_duration;
    
    if (time_remaining < 0)
      time_remaining = 0;
    
    last_read_duration = read_duration;
    
    real_nano_sleep(time_remaining);
  }
  
  return NULL;
}
