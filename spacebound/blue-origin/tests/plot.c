
#include "../include/program.h"

#include <curses.h>

#ifdef PLOT_MODE
#undef fgets
#endif

local bool initialized;
local float ordinate_min = -2;
local float ordinate_max = 20;
local int plot_index;
local int data;

local int left_offset = 4;

local Mutex input_lock = PTHREAD_MUTEX_INITIALIZER;
local struct pollfd intercept;
local char plot_input[32];

void plot_fgets(char * s, int n, FILE * stream) {
  
  if (!initialized)
    return;
  
  //fgets(s, n, stream);
  
  aquire(&input_lock) {
    strcpy(s, plot_input);
  }
}

void consider_plotting_sensors(int duration) {
  local int plot_delay = 1000;
  
  if (!schedule -> print_sensors || (plot_delay -= duration) > 0)
    return;
  
  plot_delay = console_print_interval;
  
  if (unlikely(!initialized)) {
    
    intercept.fd = 0;
    intercept.events = POLLIN;
    
    pthread_mutex_lock(&input_lock);
    
    initscr();
    noecho();
    
    erase();
    
    move(LINES - 2, 0);             // x-axis
    hline(ACS_HLINE, COLS);         // ------
    
    move(0, left_offset);           // y-axis
    vline(ACS_VLINE, LINES - 2);    // ------
    
    mvaddch(LINES - 2, left_offset, ACS_BTEE);    // (0, 0)
    
    mvaddch(LINES - 1, 0, '$');
    
    initialized = true;
  }
  
  mvprintw(        2, 1, "%.1f ", ordinate_max);
  mvprintw(LINES - 5, 1, "%.1f ", ordinate_min);
  
  for (iterate(active_sensors, Sensor *, sensor)) {
    
    when (sensor -> print);
    
    for (int stream = 0; stream < sensor -> data_streams; stream++) {
      
      Output * output = &sensor -> outputs[stream];
      
      when (output -> print);
      
      int vHeight = LINES - 3;
      
      int value = round(vHeight * (output -> measure) / (ordinate_max - ordinate_min) + ordinate_min);
      
      mvprintw(LINES - 3, COLS - 16, "%d ", value);
      mvaddstr(LINES - 3 - value, plot_index + 3, "*");
      
      //printf("Redirect: %f  \n", output -> measure);
    }
  }
  
  move(LINES - 1, 2);
  clrtoeol();            // -----------
  
  if (poll(&intercept, 1, 0) > 0) {
    
    int bytes = read(0, plot_input + strlen(plot_input), 32 - strlen(plot_input));
    plot_input[bytes] = '\0';
    
    if (plot_input[strlen(plot_input) - 1] == '\n') {
      
      pthread_mutex_unlock(&input_lock);    // give user.c an opportunity to process the input
      pthread_mutex_lock  (&input_lock);    // -----------------------------------------------
      
      plot_input[0] = '\0';
      
      move(LINES - 6, 10);
      printw("sent");
    }
  }
  
  printw("%2d %d %s", strlen(plot_input), plot_input[strlen(plot_input) - 1], plot_input);
  
  refresh();
  plot_index = (plot_index + 1) % COLS;
  
  if (plot_index < 10) return;
  
  endwin();
  exit_printing(EXIT_SUCCESS, "plot");
}
