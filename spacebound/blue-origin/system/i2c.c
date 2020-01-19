
#include "../include/program.h"

local int8 handles[0x7F];

local void * i2c_main();

i2c_device * create_i2c_device(Sensor * sensor, i2c_reader reader) {
  // creates an i2c device, adding it to the device list
  
  i2c_device * i2c = calloc(1, sizeof(*i2c));
  
  i2c -> sensor   = sensor;
  i2c -> read     = reader;
  
  i2c -> address           = sensor -> address;
  i2c -> hertz             = sensor -> hertz;
  i2c -> hertz_denominator = sensor -> hertz_denominator;
  
  if (i2c -> hertz)
    i2c -> interval = 1000 / (i2c -> hertz);
  
  if (!i2c -> hertz_denominator)
    i2c -> hertz_denominator = 1;
  
  
  // see if we need to open another i2c instance
  if (handles[i2c -> address] == -1) {
    i2c -> handle = i2cOpen(1, i2c -> address, 0);
    handles[i2c -> address] = i2c -> handle;
  } else {
    i2c -> handle = handles[i2c -> address];
  }
  
  // print a nice message for the user
  printf("Started " GREEN "%s " RESET "at " YELLOW "%dHz " RESET "on " BLUE "0x%x " RESET,
	 sensor -> name, sensor -> hertz, i2c -> address);
  if (sensor -> print) printf("with " MAGENTA "printing\n" RESET);
  else                 printf("\n");
  
  return i2c;
}

uint8 i2c_read_byte(i2c_device * dev, uint8 reg) {
  // reads a single byte from an i2c device
  
  return i2cReadByteData(dev -> handle, reg);
}

bool i2c_read_bytes(i2c_device * dev, uint8 reg, uint8 * buf, char n) {
  // reads up to 32 bytes from an i2c device
  
  if (i2cReadI2CBlockData(dev -> handle, reg, buf, n) < 0) {
    
    if (console_error_messages)
      printf(RED "Could not read bytes from " YELLOW "%s\n" RESET, dev -> sensor -> name);
    
    fprintf(schedule -> i2c_error_log, "%f%s\tread\t0x%u\t%d\n", time_passed(), time_unit, reg, n);
    
    return false;
  }
  return true;
}

bool i2c_raw_read(i2c_device * dev, uint8 * buf, char n) {
  // reads up to 32 bytes from an i2c device, without asking for a particular register
  
  if (i2cReadDevice(dev -> handle, buf, n)) {
    
    if (console_error_messages)
      printf(RED "Could not read raw bytes from " YELLOW "%s\n" RESET, dev -> sensor -> name);
    
    fprintf(schedule -> i2c_error_log, "%f%s\tread\t----\t%d\n", time_passed(), time_unit, n);
    
    return false;
  }
  return true;
}

bool i2c_raw_write(i2c_device * dev, uint8 * buf, char n) {
  // writes up to 32 bytes from an i2c device, without specifying a particular register
  
  if (i2cWriteDevice(dev -> handle, buf, n)) {
    
    if (console_error_messages)
      printf(RED "Could not write raw bytes from " YELLOW "%s\n" RESET, dev -> sensor -> name);
    
    fprintf(schedule -> i2c_error_log, "%f%s\twrite\t----\t%d\n", time_passed(), time_unit, n);
    
    return false;
  }
  return true;
}

bool i2c_write_byte(i2c_device * dev, uint8 reg, uint8 value) {
  // writes a byte to the i2c device
  
  if (i2cWriteByteData(dev -> handle, reg, value) < 0) {
    
    if (console_error_messages)
      printf(RED "Could not write byte to " YELLOW "%s\n" RESET, dev -> sensor -> name);
    
    fprintf(schedule -> i2c_error_log, "%f%s\twrite\t0x%u\t1\n", time_passed(), time_unit, reg);
    
    return false;
  }
  return true;
}

bool i2c_write_bytes(i2c_device * dev, uint8 reg, uint8 * buf, char n) {
  // writes up to 32 bytes to the i2c device
  
  if (i2cWriteI2CBlockData(dev -> handle, reg, buf, n)) {
    
    if (console_error_messages)
      printf(RED "Could not write bytes to " YELLOW "%s\n" RESET, dev -> sensor -> name);
    
    fprintf(schedule -> i2c_error_log, "%f%s\twrite\t0x%u\t%d\n", time_passed(), time_unit, reg, n);
    
    return false;
  }
  return true;
}


void i2c_close(i2c_device * i2c) {
  // closes and frees the i2c device
  fclose(i2c -> log);
  
  // only close i2c addresses once
  if (handles[i2c -> handle] != -1 && i2c -> handle >= 0x03) {
    i2cClose(i2c -> handle);
    handles[i2c -> handle] = -1;
  }
  free(i2c);
}

void init_i2c() {
  // initialize i2c data structures
  
  schedule -> i2c_devices = list_create();
  schedule -> i2c_thread  = malloc(sizeof(*schedule -> i2c_thread));
  
  memset(handles, -1, 0x7F);    // 0x03 through 0x7F are available
  memset(handles,  0,    2);    // 0x00 through 0x02 are reserved
  
  schedule -> i2c_error_log = safe_open("./logs/errors-i2c.log", "a");
  schedule ->   control_log = safe_open("./logs/control.log"   , "a");
  
  fprintf(schedule ->   control_log, GRAY "Time\tEvent\tInstance\tValue\n" RESET);
  fprintf(schedule -> i2c_error_log, RED  "Time\tType\tRegister\tBytes\n"  RESET);
}

void drop_i2c() {
  // frees everything associated with the i2c system
  
  list_delete(schedule -> i2c_devices);      // note that this kills
  schedule -> i2c_devices = NULL;
  
  blank(schedule -> i2c_thread);
  
  fclose(schedule -> i2c_error_log);
  fclose(schedule -> control_log);    // change if not i2c-bound
}

void start_i2c() {
  
  if (!schedule -> i2c_active) return;
  
  printf("\nStarting i2c schedule with " MAGENTA "%d " RESET "events\n\n", schedule -> i2c_devices -> size);
  
  if (schedule -> print_sensors) {
    
    printf(GREY "Time     ");
    
    for (iterate(active_sensors, Sensor *, sensor)) {
      if (sensor -> print) {
        for (int stream = 0; stream < sensor -> data_streams; stream++) {
          
          Output * output = &sensor -> outputs[stream];
          
          when (output -> print);
          
          printf("%s%-*s", output -> print_code, 5 + output -> print_places + strlen(output -> unit), output -> name);
        }
      }
    }
    
    printf("\n" RESET);
  }
  
  // create i2c thread
  if (pthread_create(schedule -> i2c_thread, NULL, i2c_main, NULL)) {
    printf(RED "Could not start i2c thread\n" RESET);
    return;
  }
}

void * i2c_main() {
  
  prctl(PR_SET_NAME, "i2c sched", NULL, NULL, NULL);
  
  FILE * i2c_log = safe_open("logs/i2c.log", "a");
  fprintf(i2c_log, GRAY "Read duration [ns]\n" RESET);
  
  long bus_interval    = schedule -> i2c_interval;
  long bus_interval_ms = schedule -> i2c_interval / (long) 1E6;
  
  long last_read_duration = 0;    // tracks time taken to read i2c bus
  
  while (!schedule -> term_signal) {
    
    // get time before we perform the read
    struct timespec pre_read_time;
    clock_gettime(CLOCK_REALTIME, &pre_read_time);
    
    fprintf(i2c_log, "%ld\n", last_read_duration);
    
    
    // pulse pins
    for (iterate(schedule -> pulse_pins, Pin *, pin)) {
      
      //printf("DEBUG WIRE: %d " YELLOW "%d" RESET "\n", pin -> broadcom, pin -> ms_until_pulse_completes);
      
      when (pin -> ms_until_pulse_completes);
      
      pin -> ms_until_pulse_completes -= bus_interval_ms;
      
      if (pin -> ms_until_pulse_completes <= 0) {
        pin_set(pin -> broadcom, pin -> pulse_final_state);
        pin -> ms_until_pulse_completes = 0;
      }
    }
    
    
    // change states
    for (iterate(state_delays -> all, StateDelay *, state_delay)) {
      
      //printf("DEBUG STATE: %s " CYAN "%d" RESET "\n", state_delay -> state, state_delay -> ms_remaining);
      
      when (state_delay -> ms_remaining);
      
      state_delay -> ms_remaining -= bus_interval_ms;
      
      if (state_delay -> ms_remaining <= 0) {
        if (state_delay -> entering) enter_state(state_delay -> state);
        else                         leave_state(state_delay -> state);
        state_delay -> ms_remaining = 0;
      }
      
    }
    
    // read sensors
    for (iterate(schedule -> i2c_devices, i2c_device *, i2c)) {
      
      i2c -> count += bus_interval_ms;
      
      if (i2c -> count == (i2c -> interval) * (i2c -> hertz_denominator) || i2c -> reading) {
        
        (i2c -> read)(i2c);
        
        i2c -> count = 0;
      }
    }
    
    // print to console
    if (schedule -> print_sensors) {
      
      printf(GREY "%.3lf%s  ", time_passed(), time_unit);
      
      for (iterate(active_sensors, Sensor *, sensor)) {
        
        when (sensor -> print);
        
        for (int stream = 0; stream < sensor -> data_streams; stream++) {
          
          Output * output = &sensor -> outputs[stream];
          
          when (output -> print);
          
          if (output -> measure >= 0)
            printf(" ");
          
          printf("%s%.*f%s  ", output -> print_code, output -> print_places, output -> measure, output -> unit);
        }
      }
      
      printf("\n" RESET);
    }
    
    // figure out how long to sleep
    long read_duration = real_time_diff(&pre_read_time);
    
    long time_remaining = bus_interval - read_duration;
    
    if (time_remaining < 0)
      time_remaining = 0;               // taking too long to read!
    
    last_read_duration = read_duration;
    
    real_nano_sleep(time_remaining);   // interval minus time it took to read sensors
  }
  
  fclose(i2c_log);  
  return NULL;
}
