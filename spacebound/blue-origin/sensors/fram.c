

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ds32.h"
#include "fram.h"

#include "../system/color.h"
#include "../system/file.h"
#include "../system/i2c.h"

void free_fram(Sensor * fram);
bool read_fram(i2c_device * fram_i2c);

uint16 fram_head;

Sensor * init_fram(Sensor * fram) {
  
  fram -> name = "FRAM";
  fram -> free = free_fram;
  
  fram -> i2c = create_i2c_device(fram, read_fram);
  printf("logged in logs/fram.log\n");
  printf("A storage medium\n\n");
  
  fram -> i2c -> log = safe_open("logs/fram.log", "a");
  
  setlinebuf(fram -> i2c -> log);    // write out every read
  
  fprintf(fram -> i2c -> log, GRAY "\n\nFRAM\nStart time %s\n" RESET, formatted_time);
  
  return fram;
}

void write_fram_data(i2c_device * fram_i2c, uint16 address, uint8 * message) {
  // writes up to 29 bytes to the FRAM
  
  uint8 LSB = (uint8) (0xFF & address);
  uint8 MSB = (uint8) (0x7F & (address >> 8));    // force address to be 15 bit

  char buffer[32];                 // LSB message NULL
  buffer[0] = MSB;
  buffer[1] = LSB;
  strncpy(buffer + 2, message, 29);
  buffer[31] = '\0';               // Null terminated in case of mesage cuttoff
  
  char num = 2 + strlen(buffer + 1);    // address could have 0x0 (which equals \0)
  
  i2c_raw_write(fram_i2c, buffer, num);
  
  fram_head = address + num;
}

void read_fram_data(i2c_device * fram_i2c, uint16 address, uint8 * dest, short n) {
  // reads any number of bytes from FRAM

  uint8 LSB = (uint8) (0xFF & address);
  uint8 MSB = (uint8) (0x7F & (address >> 8));    // force address to be 15 bit

  uint8 request[3] = {
    MSB, LSB, 'a'
  };
  
  //  i2cSwitchCombined(1);    // merge write-read patterns
  
  i2c_raw_write(fram_i2c, request, 3);
  //i2c_raw_read (fram_i2c,    dest, n);
  
  for (short i = 0; i < n; i++) {
    *(dest++) = (uint8) i2c_read_byte(fram_i2c, (address + i - 2) & 0xFF);
  }
  
  //  i2cSwitchCombined(0);    // dissociate write-read patterns
}

void print_byte(FILE * file, uint8 byte) {

  if (' ' <= byte && byte < '~') {
    // in readable ascii range
    fprintf(file, "%c ", byte);
    return;
  }

  char * numerals = "0123456789ABCDEF";
  
  fprintf(file, "0x%c%c ", numerals[byte >> 4], numerals[byte & 0xF]);
}

bool read_fram(i2c_device * fram_i2c) {
  
  return true;    // SHORTING
  
  static char fram_step;
  const char fram_cycle = 4;
  
  char format_length = strlen(formatted_time);
  uint16 address = fram_step * format_length;
  
  write_fram_data(fram_i2c, address, formatted_time);
  
  char read_buffer[32];
  
  read_fram_data(fram_i2c, address, read_buffer, format_length);
  
  //fprintf(fram_i2c -> log, "value: %s\n", read_buffer);

  fprintf(fram_i2c -> log, "value[ ");
  
  for (char i = 0; i < format_length; i++)
    print_byte(fram_i2c -> log, read_buffer[i]);
  
  fprintf(fram_i2c -> log, " ]\n");
  
  //i2c_read_bytes(fram_i2c, 0x00, read_raws, 7);
  
  return true;
}

void free_fram(Sensor * fram) {
  // Nothing special has to happen  
}
