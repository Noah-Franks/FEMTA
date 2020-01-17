
#include "../include/program.h"
//#include "../parser/y.tab.h"

bool console_error_messages = true;

FILE * yyin;

local void parse_args(int, char **);

int main(int nargs, char ** args) {
  
  time_start_os = time(NULL);    // OS time when system started
  //clock_gettime(CLOCK_REALTIME, &time_start_os);
  
  // start pigpio library
  if (gpioInitialise() < 0)
    exit_printing(ERROR_OPERATING_SYSTEM, "pigpio unable to start");
  
  schedule = calloc(1, sizeof(*schedule));
  
  init_color();      // init colorized printing to the console
  init_units();      // init unit conversion structures
  init_pins();       // set up gpio data structure
  init_states();
  init_one();        // set up the 1-wire data structures
  init_i2c();        // set up the i2c data structures
  init_sensors();    // set up sensor info and actions
  
  parse_args(nargs, args);
  print_config();
  
  //gpioTerminate(); exit(0);    // TEMP
  
  start_sensors();
  start_one();       // start reading the 1-wire bus
  start_i2c();       // start reading the i2c bus
  
  prctl(PR_SET_NAME, "Console", NULL, NULL, NULL);
  
  Selector * selector = create_selector(NULL);
  
  add_selector_command(selector, 'q', NULL,    flip_bool,     &reading_user_input);
  add_selector_command(selector, 'e', NULL,    flip_bool, &console_error_messages);
  add_selector_command(selector, 'm', NULL,   output_str,                    NULL);
  add_selector_command(selector, '+', NULL,  pin_set_hot,                    NULL);
  add_selector_command(selector, '-', NULL, pin_set_cold,                    NULL);
  add_selector_command(selector, 'p', NULL,   flip_print,                    NULL);
  
  reading_user_input = true;
  
  char input[32];
  while (reading_user_input) {
    fgets(input, 32, stdin);
    
    execute_selector(selector, input[0], input);
  }
  
  // tell threads to terminate
  schedule -> term_signal = true;
  
  // join with threads
  if (schedule -> i2c_active) pthread_join(*schedule -> i2c_thread, NULL);
  if (schedule -> one_active) pthread_join(*schedule -> one_thread, NULL);
  
  terminate_sensors();    // close and destroy all sensor-related structures
  terminate_i2c();        // destroy i2c bus structures
  terminate_one();        // destroy 1-wire bus structures
  
  list_destroy(schedule -> pulse_pins);
  free(schedule);
  
  // terminate pigpio library
  gpioTerminate();
  
  drop_states();
  drop_units();
  terminate_color();
  
  return EXIT_SUCCESS;
}

void parse_args(int argc, char ** argv) {
  
  if (argc == 1) {
    printf(RED "Please supply a file to run\n" RESET);
    return;
  }
  
  /* parse a file for specifications */
  
  char * filename = argv[1];
  
  printf("Parsing experiment file %s\n", filename);
  
  yyin = fopen(filename, "r");
  
  if (!yyin) {
    printf(RED "Experiment file %s does not exist\n" RESET, filename);
    exit(ERROR_EXPERIMENTER);
  }
  
  yyparse();
  return;
}
