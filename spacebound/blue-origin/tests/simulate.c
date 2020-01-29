
#include "../include/program.h"

void print_simulation_info() {
  printf(PINK "Starting Simulation\n" RESET);
  printf("  " PINK "-" RESET" Sensors and actuators will be redirected to a virtual environment\n");
  printf("  " PINK "-" RESET" a sandbox will protect the physical hardware against bugs\n\n");
}

bool simulation_report_containment_error(void * nil, ...) {
  exit_printing(ERROR_PROGRAMMER, PINK "Panic! Simulation has tried to access physical hardware!\n");
}

int return_zero() { return 0; }    // MACRO'd to replace other functions

int simulation_gpio_read(uint8 broadcom) {
  return 0;    // TODO: connect to sim
}

int simulation_gpio_write(uint8 broadcom, uint8 level) {
  return 0;    // TODO: connect to sim
}

local bool simulation_read(Sensor * sensor) {
  return false;    // TODO: connect to sim
}

bool simulation_read_i2c(i2c_device * i2c) {
  return simulation_read(i2c -> sensor);
}

bool simulation_read_one(one_device * one) {
  return simulation_read(one -> sensor);
}
