#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pigpio.h>

unsigned int SLF3_ADDRESS = 0x08;

int read_slf3(int handle, FILE *log);

int main() {

  int handle, i;
  FILE *log;
  char write_raws[2];
  
  gpioInitialise();
  log = fopen("slf3.log", "a");
  handle = i2cOpen(1, SLF3_ADDRESS, 0);

  sleep(0.5);
  if(i2cWriteByteData(handle, 0x3F, 0xF9) < 0) return 0;;
  sleep(0.5);
  
  while (i2cWriteByteData(handle, 0x36, 0x08) < 0) {
    printf("Write failed.\n");
    sleep(1);
  }
  
  printf("Started SLF3-1300F on 0x%x\n", SLF3_ADDRESS);

  for (i = 0; i < 20; i++) {
    read_slf3(handle, log);
    sleep(1);
  }

  fclose(log);
  i2cClose(handle);

  return 0;
}

int read_slf3(int handle, FILE *log) {

  char read_raws[9];
  
  if (!i2cReadDevice(handle, read_raws, 9)) return 0;

  // Flow rate scale factor: 500 (mL/min)^-1
  float flow = (read_raws[1] | (read_raws[0] << 8)) / 500.0f;
  // Temperature scale factor: 200 (deg C)^-1
  float temp = (read_raws[4] | (read_raws[3] << 8)) / 200.0f;
  int flag = read_raws[7] | (read_raws[6] << 8);
  
  int bubble = !!(flag & 0b1); // Bubble flag is in LSB of third byte -- 1 = bubble

  printf("flow (mL/min): %.4f\ttemp (C): %.4f\tbubble: %d\n", flow, temp, bubble);
  fprintf(log, "flow (mL/min): %.4f\ttemp (C): %.4f\tbubble: %d\n", flow, temp, bubble);

  return 1;
}
