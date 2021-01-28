#ifndef HEADER_GAURD_COLORS
#define HEADER_GAURD_COLORS

#include <stdbool.h>

#include "../structures/hashmap.h"

#define RED     "\e[31m"
#define GREEN   "\e[32m"
#define YELLOW  "\e[33m"
#define BLUE    "\e[34m"
#define MAGENTA "\e[35m"
#define CYAN    "\e[36m"
#define GRAY    "\e[90m"
#define RESET   "\e[39m"

void init_color();
void terminate_color();
char * get_color_by_name(char * name);

#endif
