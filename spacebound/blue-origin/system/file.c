
#include "../include/program.h"

FILE * safe_open(char * path, char * mode) {
  
  FILE * file = fopen(path, mode);
  
  if (!file)
    exit_printing(ERROR_OPERATING_SYSTEM, RED "Could not open " CYAN "%s\n", path);
  
  return file;
}

bool scan_file(char * path, char * format, ...) {
  
  FILE * file = fopen(path, "r");
  
  if (!file)
    return false;
  
  va_list args;
  va_start(args, format);
  
  vfscanf(file, format, args);
  
  va_end(args);
  fclose(file);
  return true;
}
