
#include "../include/program.h"

#ifdef DEBUG_MODE
#undef malloc                      // protect file from attacking itself
#undef calloc                      // ----------------------------------
#undef realloc                     // ----------------------------------
#undef strdup                      // ----------------------------------
#undef free                        // ----------------------------------
#undef fopen                       // ----------------------------------
#undef fclose                      // ----------------------------------
#undef yyclean                     // ----------------------------------
#endif

local int allocations = 0;         // total number of allocations
local int frees       = 0;         // total number of de-allocations
local int file_opens  = 0;         // total number of file opens
local int file_closes = 0;         // total number of file closures

void print_usage_debug_info() {    // print out the above usage statistics
  
  printf(BLUE "\nDebug Report:\n");
  
  if (file_opens == file_closes) printf(GREEN "    Disk   Success: ");
  else                           printf(  RED "    Disk   Failure: ");
  
  printf(CYAN "% 5d" RESET " fopen's and " CYAN "% 5d " RESET "fcloses's\n", file_opens, file_closes);
  
  if (allocations == frees) printf(GREEN "    Memory Success: ");
  else                      printf(  RED "    Memory Failure: ");
  
  printf(CYAN "% 5d" RESET " alloc's and " CYAN "% 5d " RESET "free's\n", allocations, frees);
  printf("\n");
}


/* debug versions of the common memory functions */

void * debug_malloc(size_t size) {
  allocations++;
  return malloc(size);
}

void * debug_calloc(size_t nmemb, size_t size) {
  allocations++;
  return calloc(nmemb, size);
}

void * debug_realloc(void * pointer, size_t size) {
  allocations++;
  frees += !!pointer;
  return realloc(pointer, size);
}

char * debug_strdup(const char * s) {
  allocations++;
  return strdup(s);
}

void debug_free(void * pointer) {
  frees += !!pointer;
  free(pointer);
}

FILE * debug_fopen(const char * pathname, const char * mode) {
  file_opens++;
  return fopen(pathname, mode);
}

int debug_fclose(FILE * stream) {
  file_closes++;
  return fclose(stream);
}

int debug_yylex_destroy() {        // count the fact that lex closes yyin
  file_closes++;
  return yylex_destroy();
}
