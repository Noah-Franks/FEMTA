
#include "../include/program.h"

extern FILE * yyin;

bool console_error_messages = true;
bool use_real_time_clock    = true;
bool print_to_console       = true;
bool allow_user_input       = true;
int  console_print_interval = 1000;    // ms between prints

int main(int nargs, char ** args) {
  
  time_start_os = time(NULL);    // OS time when system started
  //clock_gettime(CLOCK_REALTIME, &time_start_os);
  
  if (gpioInitialise() < 0)
    exit_printing(ERROR_OPERATING_SYSTEM, "pigpio unable to start");
  
  if (nargs == 1) 
    exit_printing(ERROR_EXPERIMENTER, "Please supply an experiment to run");
  
  if (!(yyin = fopen(args[1], "r")))
    exit_printing(ERROR_EXPERIMENTER, "Experiment file %s does not exist", args[1]);
  
  printf("\n");
  
  #ifdef SIMULATION_MODE
  print_simulation_info();
  #endif
  
  schedule = calloc(1, sizeof(*schedule));
  
  init_color();       // set up colorized printing to the console
  init_units();       // set up unit conversions and calibration structures
  init_one();         // set up the 1-wire data structures
  init_i2c();         // set up the i2c data structures
  init_sensors();     // set up sensor info and actions
  
  yyparse();          // execute the Exp Compiler to transform this process into an experiment
  yyclean();          // delete Exp's lexical analyzer's remaining stack and debris
  print_config();     // print sensor, trigger, and state information to the experimenter
  
  build_schedule();
  if (schedule -> i2c_active) start_i2c();       // start reading the i2c bus
  if (schedule -> one_active) start_one();       // start reading the 1-wire bus
  
  if (allow_user_input)
    present_interface();
  
  schedule -> term_signal = true;                // tell all child threads to terminate
  
  // join with threads
  if (schedule -> i2c_active) pthread_join(*schedule -> i2c_thread, NULL);
  if (schedule -> one_active) pthread_join(*schedule -> one_thread, NULL);
  
  drop_sensors();     // close and destroy all sensor-related structures
  drop_i2c();         // delete i2c bus structures
  drop_one();         // delete 1-wire bus structures
  drop_states();      // delete the state system
  drop_units();       // delete unit structures
  drop_color();       // delete color structures
  
  gpioTerminate();    // terminate pigpio library
  
  blank(schedule);
  
  #ifdef DEBUG_MODE
  print_usage_debug_info();
  #endif
  
  return EXIT_SUCCESS;
}
