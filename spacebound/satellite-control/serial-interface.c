/**
 * The following file enables serial communication with the BNO-055. 
 * Base functions come from Steven Pugia's C# library, implemented 
 * using pigpio analogs to the C#-specific commands.
 * 
 */

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pigpio.h>
#include <stdbool.h>
#include <pthread.h>

#include "unified-controller.h"
#include "serial-interface.h"
#include "linked-list.h"
#include "graphics.h"
#include "timing.h"
#include "colors.h"


#define POWER_MODE_NORMAL            0x00
#define OPERATION_MODE_CONFIG        0x00
#define BNO055_CHIP_ID_ADDR          0x00
#define BNO055_PAGE_ID_ADDR          0x07
#define BNO055_ACCEL_DATA_X_LSB_ADDR 0x08
#define OPERATION_MODE_NDOF          0x0C
#define BNO055_MAG_DATA_X_LSB_ADDR   0x0E
#define BNO055_TEMP_ADDR             0x34
#define BNO055_OPR_MODE_ADDR         0x3D
#define BNO055_PWR_MODE_ADDR         0x3E
#define BNO055_SYS_TRIGGER_ADDR      0x3F
#define BNO055_ID                    0xA0


FILE * bno_log_file;
char * bno_log_file_name = "./logs/bno-log.txt";
pthread_t bno_thread;
bool bno_termination_signal;
int bno_values_read = 0;


void write_byte(uint8_t handle, int8_t address, int8_t data, bool ack) {

  int max_attempts = 10;

  int8_t command[5] = {
    0xAA,
    0x00,
    address,
    0x01,
    data,
  };
  
  for (int attempt = 0; attempt < max_attempts; attempt++) {

    serWrite(handle, command, 5);

    if (!ack) return;

    while (serDataAvailable(handle) < 2);
    int8_t response[2];
    serRead(handle, response, 2);

    if (!(response[0] == 0xEE && response[1] == 0x07)) return;    
  }
  
}

void serial_set_mode(int8_t mode) {
  write_byte(serial_device -> uart -> serial_handle, BNO055_OPR_MODE_ADDR, mode & 0xFF, true);
}

void read_bytes(unsigned char handle, signed char address, signed char * buffer, signed char length) {

  int max_attempts = 10;
  
  signed char command[4] = {
    0xAA,
    0x01,
    //BNO055_MAG_DATA_X_LSB_ADDR & 0xFF,
    //address & 0xFF,
    //length  & 0xFF,
    address,
    length,
  };

  for (int attempt = 0; attempt < max_attempts; attempt++) {
  
    serWrite(handle, command, 4);
  
    while (serDataAvailable(handle) < 2);
    int8_t response[2];
    int read_success = serRead(handle, response, 2);
    
    //if (!(response[0] == 0xEE && response[1] == 0x07)) {
    //if (response[0] == 0xBB && response[1] == length) {
    
	while (serDataAvailable(handle) < length);
	serRead(handle, buffer, length);

	return;
	//}
	//}
  }
}

float readSerialTempData() {

  //Short this
  return 0.0;
  
  int8_t rawData[1] = {0x00,};
  
  read_bytes(serial_device -> uart -> serial_handle, BNO055_TEMP_ADDR, rawData, 1);
  
  return (float) rawData[0];
}

void read_serial_magnetometers(float * axes) {
  int length = 6;
  int8_t rawData[length];
  for (int i = 0; i < length; i++) rawData[i] = 0;
  
  read_bytes(serial_device -> uart -> serial_handle, BNO055_MAG_DATA_X_LSB_ADDR, rawData, length);
  
  axes[3];
  for (int i = 0; i < length; i += 2) {
    int entry = ((rawData[i + 1] << 8) | rawData[i]);
    if (entry > 32767) entry -= 65536;
    axes[i / 2] = entry / 16.0;
  }
}

void read_serial_accelerometers(float * axes) {
  
}

void read_serial_gyroscopes(float * axes) {

}

void * log_bno_data() {

  while (!bno_termination_signal) {

    bno_log_file = fopen(bno_log_file_name, "a");

    float log_data[50][9];

    for (uint8_t i = 0; i < 50; i++) {

      read_serial_gyroscopes(log_data[i]);
      read_serial_accelerometers(log_data[i] + 3);
      read_serial_magnetometers(log_data[i] + 6);

      fprintf(bno_log_file, "%d\t", bno_values_read++);
      for (uint8_t f = 0; f < 9; f++) fprintf(bno_log_file, "%.3f\t", log_data[i][f]);
      for (uint8_t f = 0; f < 3; f++) {
	plot_add_value(bno_gyro_plot, bno_gyro_plot -> lists[f], create_fnode(log_data[i][f    ]));
	plot_add_value(bno_acel_plot, bno_acel_plot -> lists[f], create_fnode(log_data[i][f + 3]));
	plot_add_value(bno_magn_plot, bno_magn_plot -> lists[f], create_fnode(log_data[i][f + 6]));
      }

      fprintf(bno_log_file, "\n");

      graph_plot(bno_gyro_plot);
      graph_plot(bno_acel_plot);
      graph_plot(bno_magn_plot);
      nano_sleep(100000000);
    }

    fflush(stdout);
    fclose(bno_log_file);
    
  }
  
}


bool initialize_UART(module * initialent) {
  
  initialent -> uart = malloc(sizeof(UART));
  serial_device = initialent;
  serial_device -> uart -> temperature = &readSerialTempData;

  // Reset the BNO-055
  set_voltage(serial_device -> pins + 2, 0);
  nano_sleep(10000000);
  set_voltage(serial_device -> pins + 2, 1);
  nano_sleep(650000000);
  
  serial_device -> uart -> serial_handle = serOpen("/dev/ttyAMA0", 115200, 0);

  write_byte(serial_device -> uart -> serial_handle, BNO055_PAGE_ID_ADDR, 0x00, false);
  serial_set_mode(OPERATION_MODE_CONFIG);
  nano_sleep(30000000);

  write_byte(serial_device -> uart -> serial_handle, BNO055_PAGE_ID_ADDR, 0, true);

  // Reset the BNO-055
  set_voltage(serial_device -> pins + 2, 0);
  nano_sleep(10000000);
  set_voltage(serial_device -> pins + 2, 1);
  nano_sleep(650000000);

  write_byte(serial_device -> uart -> serial_handle, BNO055_PWR_MODE_ADDR,    POWER_MODE_NORMAL, true);
  write_byte(serial_device -> uart -> serial_handle, BNO055_SYS_TRIGGER_ADDR, 0x00,              true);

  serial_set_mode(OPERATION_MODE_NDOF);
  nano_sleep(30000000);
  
  write_byte(serial_device -> uart -> serial_handle, BNO055_CHIP_ID_ADDR, 0, true);
  
  
  if (serial_device -> uart -> serial_handle < 0) return false;
  
  // Graphics memory allocation
  bno_gyro_plot = create_plot("    BNO Gyro Axes v.s. Time    ", 3);
  bno_acel_plot = create_plot("BNO Acelerometer Axes v.s. Time", 3);
  bno_magn_plot = create_plot("BNO Magnetometer Axes v.s. Time", 3);  

  bno_log_file = fopen(bno_log_file_name, "a");
  fprintf(bno_log_file, GREEN "\nRecording MPU Data\nTIME\tGyro x\tGyro y\tGyro z\tAcel x\tAcel y\tAce lz\tMagn x\tMagn y\tMagn z\n" RESET);
  fclose(bno_log_file);

  bno_termination_signal = false;
  pthread_create(&bno_thread, NULL, log_bno_data, NULL);
  
  return true;
}

void terminate_bno_logging() {
  bno_termination_signal = true;
  
  if (serial_device -> uart -> serial_handle >= 0) serClose(serial_device -> uart -> serial_handle);
}
