
#include "../include/program.h"

FILE * safe_open(char * path, char * mode) {
  
  FILE * file = fopen(path, mode);
  
  if (!file)
    exit_printing(ERROR_OPERATING_SYSTEM, RED "Could not open " CYAN "%s\n", path);
  
  return file;
}
