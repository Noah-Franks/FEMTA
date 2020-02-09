
#include "../include/program.h"

void exit_printing(int code, ...) {       // safely exit to the console after printing in color
  
  va_list args;
  va_start(args, code);
  
  char * format = va_arg(args, char *);
  
  if (code == SUCCESS) printf(GREEN);     // pick a color based on success
  else                 printf(RED);       // -----------------------------
  vprintf(format, args);
  printf(RESET "\n");
  
  va_end(args);
  
  gpioTerminate();                        // release the gpio daemon
  exit(code);                             // terminate the process
}

void print_error(char * format, ...) {    // print an error to the console
  
  if (!console_error_messages)
    return;
  
  va_list args;
  va_start(args, format);
  
  printf(RED);
  vprintf(format, args);
  printf(RESET);
  
  va_end(args);
}
