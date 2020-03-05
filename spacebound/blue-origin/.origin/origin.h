#pragma once

#include "../include/headers.h"

/* static configuration */

extern bool console_error_messages;    // whether messages should be printed to the console
extern bool use_real_time_clock;       // whether the real time clock should define expirement time
extern bool print_to_console;          // whether to print any console output
extern bool allow_user_input;          // whether to take input from the user
extern int  console_print_interval;    // the ms between each console printout

#define DEBUG_MODE                     // define to track resources
//#define SIMULATION_MODE                // define to simulate sensors
#define PLOT_MODE                      // define to redirect console outputs to plot
