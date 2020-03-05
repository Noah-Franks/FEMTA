
#include "../include/program.h"

local float plot_min = 0;
local float plot_max = 16;

typedef struct PlotPoint {
  
  int    value;
  char * color;
  
} PlotPoint;

local PlotPoint points[16];

local int compare_points(const void * a, const void * b) {
  return (((PlotPoint *) a) -> value) - (((PlotPoint *) b) -> value);
}

void consider_plotting_sensors(int duration) {
  local int print_delay = 0;
  
  if (!schedule -> print_sensors || (print_delay -= duration) > 0)
    return;
  
  print_delay = console_print_interval;
  
  struct winsize window;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);
  float plot_space = window.ws_col - 10;
  
  printf("%c[2K\r", 27);    // wipe the current line
  fflush(stdout);           // ---------------------
  
  
  // print data points
  
  printf(GREY "% 8.3lf |", time_passed());
  
  int num_points = 0;
  
  for (iterate(active_sensors, Sensor *, sensor)) {
    
    when (sensor -> print);
    
    for (int stream = 0; stream < sensor -> data_streams; stream++) {
      
      Output * output = &sensor -> outputs[stream];
      
      when (output -> print);
      
      points[num_points].value = plot_space * (output -> measure - plot_min) / (plot_max - plot_min);
      points[num_points].color = output -> print_code;
      
      num_points++;
    }
  }
  
  qsort(points, num_points, sizeof(*points), compare_points);
  
  int offset = 0;
  for (int i = 0; i < num_points; i++) {
    
    int value = points[i].value;
    int place = value - offset - 1;
    
    when (place > 0 && value <= plot_space);
    
    printf("%s%*c*", points[i].color, place, ' ');
    offset = value;
  }
  
  
  // print actual values at the bottom
  
  printf(GREY "\n% 8.3lf  ", time_passed());
  
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
  
  fflush(stdout);
}
