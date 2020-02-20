
#include "../include/program.h"

local void * one_main();

one_device * create_one_device(Sensor * sensor, char * path, char * log_path, one_reader read) {
  
  one_device * one = calloc(1, sizeof(*one));
  
  one -> sensor = sensor;
  one -> path   = path;
  one -> log    = safe_open(log_path, "a");
  one -> read   = read;
  
  one -> hertz_numerator   = sensor -> hertz_numerator;
  one -> hertz_denominator = sensor -> hertz_denominator;
  
  if (one -> hertz_numerator)
    one -> interval = schedule -> one_cycle / (one -> hertz_numerator);
  
  // print a nice message for the user
  printf("Started " GREEN "%s " RESET "at " YELLOW "%d", sensor -> name, one -> hertz_numerator);
  if (one -> hertz_denominator != 1) printf("/%d", one -> hertz_denominator);
  printf("Hz " RESET "via " BLUE "%s " RESET, path);
  if (sensor -> print) printf("with " MAGENTA "printing" RESET);
  printf("\nlogged in %s\n", log_path);
  
  #ifdef SIMULATION_MODE
  one -> read = simulation_read_one;
  #endif
  
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

void drop_one() {
  
  list_delete(schedule -> one_devices);
  schedule -> one_devices = NULL;
  
  blank(schedule -> one_thread);
}

void start_one() {
  
  printf("\nStarting 1-wire schedule with " MAGENTA "%d " RESET "events\n", schedule -> one_devices -> elements);
  
  // create 1-wire thread
  if (pthread_create(schedule -> one_thread, NULL, one_main, NULL)) {
    printf(RED "Could not start 1-wire thread\n" RESET);
    return;
  }
}

local void * one_main() {
  
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
        
        schedule -> last_one_dev = one;
      }
    }
    
    
    // figure out how long to sleep
    
    long read_duration = real_time_diff(&pre_read_time);
    
    long time_remaining = one_interval - read_duration;
    
    if (time_remaining < 0)
      time_remaining = 0;
    
    last_read_duration = read_duration;
    
    nano_sleep(time_remaining);
  }
  
  return NULL;
}
