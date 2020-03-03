
#include "../include/program.h"

#include <curses.h>

local bool initialized;
local float ordinate_min = -2;
local float ordinate_max = 20;
local int plot_index;
local int data;

void consider_plotting_sensors(int duration) {
  local int plot_delay = 1000;
  
  if (!schedule -> print_sensors || (plot_delay -= duration) > 0)
    return;
  
  plot_delay = console_print_interval;
  
  if (unlikely(!initialized)) {
    initscr();
    erase();
    initialized = true;
    move(LINES - 2, 0);
    hline(ACS_HLINE, COLS);
    move(0, 8);
    vline(ACS_VLINE, LINES - 2);
    mvaddch(LINES - 1, 0, '$');
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
  refresh();
  plot_index = (plot_index + 1) % COLS;
  
  if (plot_index < 8) return;
  
  endwin();
  exit_printing(EXIT_SUCCESS, "plot");
}
