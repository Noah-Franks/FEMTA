
#include "../include/program.h"

#ifdef DEBUG_MODE
#undef malloc         // protect file from attacking itself
#undef calloc         // ----------------------------------
#undef strdup         // ----------------------------------
#undef free           // ----------------------------------
#undef fopen          // ----------------------------------
#undef fclose         // ----------------------------------
#undef yyclean        // ----------------------------------
#endif

local int allocations = 0;
local int frees       = 0;
local int file_opens  = 0;
local int file_closes = 0;

void * debug_malloc(size_t size) {
  allocations++;
  return malloc(size);
}

void * debug_calloc(size_t nmemb, size_t size) {
  allocations++;
  return calloc(nmemb, size);
}

char * debug_strdup(const char * s) {
  allocations++;
  return strdup(s);
}

void debug_free(void * pointer) {
  frees += pointer != NULL;
  free(pointer);
}

FILE * debug_fopen(const char * pathname, const char * mode) {
  file_opens++;
  //printf("Opened %s\n", pathname);
  return fopen(pathname, mode);
}

int debug_fclose(FILE * stream) {
  file_closes++;
  return fclose(stream);
}

int debug_yylex_destroy() {
  file_closes++;
  return yylex_destroy();
}

void print_usage_debug_info() {
  
  printf(BLUE "\nDebug Report:\n");
  
  if (file_opens == file_closes) printf(GREEN "    Disk   Success: ");
  else                           printf(  RED "    Disk   Failure: ");
  
  printf(CYAN "% 4d" RESET " fopen's and " CYAN "% 4d " RESET "fcloses's\n", file_opens, file_closes);
  
  if (allocations == frees) printf(GREEN "    Memory Success: ");
  else                      printf(  RED "    Memory Failure: ");
  
  printf(CYAN "% 4d" RESET " alloc's and " CYAN "% 4d " RESET "free's\n", allocations, frees);
}
