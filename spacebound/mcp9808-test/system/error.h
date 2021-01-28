#ifndef HEADER_GAURD_ERROR
#define HEADER_GAURD_ERROR

#define ERROR_PROGRAMMER       1
#define ERROR_OPERATING_SYSTEM 2
#define ERROR_EXPERIMENTER     3

void exit_printing(char * message, int error_code);

#endif
