
#include "../include/program.h"

typedef struct PlotPoint {
  
  int    value;      // integer number of spaces
  char * color;      // color to print
  int    places;     // bottom: number of decimal places
  float  measure;    //         actual measure observed
  char * unit;       //         measure unit
  
} PlotPoint;

local PlotPoint points[8];        // stream measures to print
local float     plot_min = 0;     // plot range
local float     plot_max = 45;   //   points outside aren't printed

local int compare_points(const void * a, const void * b) {
  return (((PlotPoint *) a) -> value) - (((PlotPoint *) b) -> value);
}

void consider_plotting_sensors(int duration) {    // replaces consider_printing_sensors()
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
      
      PlotPoint * point = &points[num_points++];
      
      point -> value   = plot_space * (output -> measure - plot_min) / (plot_max - plot_min);
      point -> color   = output -> print_code;
      point -> places  = output -> print_places;
      point -> measure = output -> measure;
      point -> unit    = output -> unit;
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
  
  for (int i = 0; i < num_points; i++) {
    
    PlotPoint * point = &points[i];
    
    printf("%s% *.*f%s ", point -> color, point -> places + strlen(point -> unit) + 4,
           point -> places, point ->  measure, point -> unit);
  }
  
  fflush(stdout);
}
