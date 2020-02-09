#pragma once

// Error codes specify the status of the process upon
// exit. As is customery, nonzero is bad.

#define SUCCESS                0    // process ended without issue
#define ERROR_PROGRAMMER       1    // some .c file has an issue
#define ERROR_OPERATING_SYSTEM 2    // unrecoverable OS failure
#define ERROR_EXPERIMENTER     3    // user .e file has an issue
