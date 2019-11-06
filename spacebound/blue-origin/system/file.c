
#include <stdio.h>
#include <stdlib.h>

#include "color.h"
#include "error.h"

FILE * safe_open(char * path, char * mode) {

  FILE * file = fopen(path, mode);
  
  if (!file) {
    printf(RED "Could not open " CYAN "%s\n", path);
    exit_printing("Could not open a necessary file", ERROR_OPERATING_SYSTEM);
  }
  
  return file;
}
