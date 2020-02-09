
#include "../include/program.h"

#define I2C_NOT_REGISTERED -1    // i2c address available, but not registered
#define I2C_NOT_ALLOWED    -2    // i2c address not allowed

#ifdef SIMULATION_MODE           // prevent preprocessor attack
#undef i2c_read_byte             // ---------------------------
#undef i2c_read_bytes            // ---------------------------
#undef i2c_raw_read              // ---------------------------
#undef i2c_raw_write             // ---------------------------
#undef i2c_write_byte            // ---------------------------
#undef i2c_write_bytes           // ---------------------------
#endif

local int8 handles[0x7F];        // pigpio handle to i2c address table

local void * i2c_main();

i2c_device * create_i2c_device(Sensor * sensor, i2c_reader reader) {       // create and register an i2c dev
  
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
  
  if (handles[i2c -> address] == I2C_NOT_REGISTERED &&                     // register new i2c instance
      handles[i2c -> address] != I2C_NOT_ALLOWED      )                    // -------------------------
    handles[i2c -> address] = i2cOpen(1, i2c -> address, 0);               // -------------------------
  
  i2c -> handle = handles[i2c -> address];
  
  // print a nice message for the user
  printf("Started " GREEN "%s " RESET "at " YELLOW "%dHz " RESET "on " BLUE "0x%x " RESET,
         sensor -> name, sensor -> hertz, i2c -> address);
  if (sensor -> print) printf("with " MAGENTA "printing\n" RESET);
  else                 printf("\n");
  
  #ifdef SIMULATION_MODE
  i2c -> read = simulation_read_i2c;                                       // connect to simulation instead
  #endif
  
  return i2c;
}

uint8 i2c_read_byte(i2c_device * dev, uint8 reg) {                         // read a single byte
  return i2cReadByteData(dev -> handle, reg);
}

bool i2c_read_bytes(i2c_device * dev, uint8 reg, uint8 * buf, char n) {    // read up to 32 bytes
  
  if (i2cReadI2CBlockData(dev -> handle, reg, buf, n) < 0) {
    print_error("Could not read bytes from " YELLOW "%s\n", dev -> sensor -> code_name);
    fprintf(schedule -> i2c_error_log, "%f%s\tread\t0x%u\t%d\n", time_passed(), time_unit, reg, n);
    return false;
  }
  return true;
}

bool i2c_raw_read(i2c_device * dev, uint8 * buf, char n) {                 // read up to 32 bytes, without 
                                                                           // specifying a particular register
  if (i2cReadDevice(dev -> handle, buf, n)) {
    print_error("Could not read raw bytes from " YELLOW "%s\n", dev -> sensor -> code_name);
    fprintf(schedule -> i2c_error_log, "%f%s\tread\t----\t%d\n", time_passed(), time_unit, n);
    return false;
  }
  return true;
}

bool i2c_raw_write(i2c_device * dev, uint8 * buf, char n) {                 // write up to 32 bytes, without
                                                                            // specifying a particular register
  if (i2cWriteDevice(dev -> handle, buf, n)) {
    print_error("Could not write raw bytes from " YELLOW "%s\n", dev -> sensor -> code_name);
    fprintf(schedule -> i2c_error_log, "%f%s\twrite\t----\t%d\n", time_passed(), time_unit, n);
    return false;
  }
  return true;
}

bool i2c_write_byte(i2c_device * dev, uint8 reg, uint8 value) {             // write a byte
  
  if (i2cWriteByteData(dev -> handle, reg, value) < 0) {
    print_error("Could not write byte to " YELLOW "%s\n", dev -> sensor -> code_name);
    fprintf(schedule -> i2c_error_log, "%f%s\twrite\t0x%u\t1\n", time_passed(), time_unit, reg);
    return false;
  }
  return true;
}

bool i2c_write_bytes(i2c_device * dev, uint8 reg, uint8 * buf, char n) {    // write up to 32 bytes
  
  if (i2cWriteI2CBlockData(dev -> handle, reg, buf, n)) {
    print_error(RED "Could not write bytes to " YELLOW "%s\n" RESET, dev -> sensor -> code_name);
    fprintf(schedule -> i2c_error_log, "%f%s\twrite\t0x%u\t%d\n", time_passed(), time_unit, reg, n);
    return false;
  }
  return true;
}

void i2c_close(i2c_device * i2c) {                                          // closes and frees the i2c device
  
  fclose(i2c -> log);
  i2c -> log = NULL;
  
  if (handles[i2c -> address] >= 0) {                                       // only close once
    handles[i2c -> address] = I2C_NOT_REGISTERED;
    i2cClose(i2c -> handle);
  }
  free(i2c);
}

void init_i2c() {                                                           // initialize i2c data structures
  
  schedule -> i2c_devices = list_create();
  schedule -> i2c_thread  = malloc(sizeof(*schedule -> i2c_thread));
  
  memset(handles, I2C_NOT_REGISTERED, 0x7F);                                // 0x03 through 0x7F are available
  memset(handles, I2C_NOT_ALLOWED   , 0x02);                                // 0x00 through 0x02 are reserved
  
  schedule -> i2c_error_log = safe_open("./logs/errors-i2c.log", "a");
  schedule ->   control_log = safe_open("./logs/control.log"   , "a");
  
  fprintf(schedule -> i2c_error_log, RED  "Time\tType\tRegister\tBytes\n"  RESET);
  fprintf(schedule ->   control_log, GRAY "Time\tEvent\tInstance\tValue\n" RESET);
}

void drop_i2c() {                                                           // delete the i2c system's structures
  
  list_delete(schedule -> i2c_devices);                                     // note that this kills
  schedule -> i2c_devices = NULL;
  
  blank(schedule -> i2c_thread);
  
  fclose(schedule -> i2c_error_log);
  fclose(schedule ->   control_log);                                        // change if not i2c-bound
}

void start_i2c() {                                                          // start the i2c thread
  
  if (!schedule -> i2c_active)                                              // no i2c sensors were requested
    return;                                                                 // -----------------------------
  
  printf("\nStarting i2c schedule with " MAGENTA "%d " RESET "events\n\n", schedule -> i2c_devices -> elements);
  
  if (schedule -> print_sensors) {
    
    printf(GREY "    Time  ");
    
    for (iterate(active_sensors, Sensor *, sensor)) {
      if (sensor -> print) {
        for (int stream = 0; stream < sensor -> data_streams; stream++) {
          
          Output * output = &sensor -> outputs[stream];
          
          when (output -> print);
          
          printf("%s%*s  ", output -> print_code, 
                 5 + output -> print_places + strlen(output -> unit), 
                 output -> nice_name);
        }
      }
    }
    
    printf("\n" RESET);
  }
  
  if (pthread_create(schedule -> i2c_thread, NULL, i2c_main, NULL))         // create i2c thread
    printf(RED "Could not start i2c thread\n" RESET);                       // -----------------
}

void * i2c_main() {
  
  prctl(PR_SET_NAME, "i2c sched", NULL, NULL, NULL);
  
  FILE * i2c_log = safe_open("logs/i2c.log", "a");
  fprintf(i2c_log, GRAY "Read duration [ns]\n" RESET);
  
  long bus_interval    = schedule -> i2c_interval;
  long bus_interval_ms = schedule -> i2c_interval / 1000000;
  
  long last_read_duration = 0;                                              // tracks time taken to read i2c bus
  long print_delay        = bus_interval_ms;                                // time before next console print
  
  while (!schedule -> term_signal) {
    
    struct timespec pre_read_time;                                          // get time before we perform the read
    clock_gettime(CLOCK_REALTIME, &pre_read_time);                          // -----------------------------------
    
    fprintf(i2c_log, "%ld\n", last_read_duration);
    
    process_pin_queue  (bus_interval_ms);                                   // process gpios waiting for changes
    process_state_queue(bus_interval_ms);                                   // process states waiting for changes
    
    for (iterate(schedule -> i2c_devices, i2c_device *, i2c)) {             // read sensors
      
      i2c -> count += bus_interval_ms;
      
      if (i2c -> count == (i2c -> interval) * (i2c -> hertz_denominator) || i2c -> reading) {
        
        (i2c -> read)(i2c);
        
        i2c -> count = 0;
        
        schedule -> last_i2c_dev = i2c;
      }
    }
    
    if (schedule -> print_sensors && (print_delay -= bus_interval_ms) <= 0) {    // print to console
      
      print_delay = console_print_interval;
      
      printf(GREY "% 7.3lf%s  ", time_passed(), time_unit);
      
      for (iterate(active_sensors, Sensor *, sensor)) {
        
        when (sensor -> print);
        
        for (int stream = 0; stream < sensor -> data_streams; stream++) {
          
          Output * output = &sensor -> outputs[stream];
          
          when (output -> print);
          
          printf("%s% *.*f%s  ", output -> print_code,
                 output -> print_places + strlen(output -> unit) + 4,
                 output -> print_places,
                 output -> measure, output -> unit);
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
    
    nano_sleep(time_remaining);   // interval minus time it took to read sensors
  }
  
  fclose(i2c_log);  
  return NULL;
}
