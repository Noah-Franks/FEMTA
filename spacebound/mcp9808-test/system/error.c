
#include <stdio.h>
#include <stdlib.h>

#include "color.h"
#include "error.h"

void exit_printing(char * message, int error_code) {
  printf(RED "%s\n" RESET, message);
  exit(error_code);
}
