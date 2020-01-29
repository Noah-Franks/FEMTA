
#include "../include/program.h"

char formatted_time[32];

local bool read_ds32(i2c_device * ds32_i2c);
local void set_time_ds32(Sensor * ds32);

local void schedule_tick(int gpio, int level, uint32 tick) {
  schedule -> interrupts++;
}

Sensor * init_ds32(Sensor * ds32) {
  
  ds32 -> name = "DS3231N";
  ds32 -> i2c = create_i2c_device(ds32, read_ds32);
  printf("logged in logs/ds32.log\n");
  printf("A real time clock\n\n");
  
  ds32 -> i2c -> log = safe_open("logs/ds32.log", "a");
  
  setlinebuf(ds32 -> i2c -> log);    // write out every read
  
  //set_time_ds32(ds32);
  
  // set up output data streams
  
  Output * time_output = &ds32 -> outputs[DS32_MEASURE_TIME];
  Output * temp_output = &ds32 -> outputs[DS32_MEASURE_TEMPERATURE];
  
  if (!ds32 -> outputs[DS32_MEASURE_TIME].series) {
    
    Calibration * calibration = calloc(1, sizeof(*calibration));
    
    calibration -> curve     = strdup("poly");
    calibration -> unit_from = strdup( "raw");
    calibration -> unit_to   = strdup(   "s");
    calibration -> target    = strdup("Time");
    calibration -> constants = list_from(2, numeric_from_decimal(0.0009765625f), numeric_from_decimal(0.0f));
    
    time_output -> series = list_from(1, series_element_from_calibration(calibration));
    time_output -> unit   = strdup("s");
  }
  
  fprintf(ds32 -> i2c -> log, GREEN "\n\nDS3231N\nExperiment Duration [%s]\tTemperature [%s]\tHuman Time\n" RESET,
	  time_output -> unit, temp_output -> unit);
  
  
  /* Ask the ds32 to start emitting square waves at a rate of 1.024kHz.
   * The bits, from left to right, with the brackets going in binary order
   * 
   *  7: Let oscillator use battery        {no, yes} (Crazy to not say 'yes')
   *  6: Enable square wave                {no, yes} (We use this for interrupts)
   *  5: Immediately adjust to temperature {no, yes} (Happens every 64s anyways, 2ms delay if yes)
   * 34: Frequency for square wave         {1Hz, 1.024kHz, 4.096kHz, 8.192kHz} (01 yields sub-ms)
   *  2: What to use the SQR pn for        {square waves, alarms}
   *  1: Enable Alarm 2?                   {no, yes}
   *  0: Enable Alarm 1?                   {no, yes}
   */
  
  if (!i2c_write_byte(ds32 -> i2c, 0x0E, 0b01101000))
    printf(RED "Could not enable ds32 square waves!\n" RESET);
  
  gpioSetISRFunc(20, RISING_EDGE, 0, schedule_tick);    // start counting interrupts
  read_ds32(ds32 -> i2c);                               // get human time before other sensors are created
  
  schedule -> interrupt_interval = series_compute(time_output -> series, 1.0f);
  time_unit = time_output -> unit;
  
  return ds32;
}

bool read_ds32(i2c_device * ds32_i2c) {
  
  Sensor * ds32 = ds32_i2c -> sensor;
  
  uint8 read_raws[9];
  
  if (!i2c_read_bytes(ds32_i2c, 0x00, read_raws + 0, 7)) return false;
  if (!i2c_read_bytes(ds32_i2c, 0x11, read_raws + 7, 2)) return false;
  
  char seconds_ones = '0' + ((0b00001111 & read_raws[0]) >> 0);    // 0 tens ones
  char seconds_tens = '0' + ((0b01110000 & read_raws[0]) >> 4);    // -----------
  
  char minutes_ones = '0' + ((0b00001111 & read_raws[1]) >> 0);    // 0 tens ones
  char minutes_tens = '0' + ((0b01110000 & read_raws[1]) >> 4);    // -----------
  
  char hours_ones   = '0' + ((0b00001111 & read_raws[2]) >> 0);    // 0 am/pm 0 tens ones
  char hours_tens   = '0' + ((0b00010000 & read_raws[2]) >> 4);
  char meridian     = 'a' + ((0b00100000 & read_raws[2]) >> 5) * ('p' - 'a');
  
  char date_ones    = '0' + ((0b00001111 & read_raws[4]) >> 0);    // 00 tens ones
  char date_tens    = '0' + ((0b00110000 & read_raws[4]) >> 4);    // ------------
  
  char month_ones   = '0' + ((0b00001111 & read_raws[5]) >> 0);    // c 00 tens ones
  char month_tens   = '0' + ((0b00010000 & read_raws[5]) >> 4);    // --------------
  char century      = '0' + ((0b10000000 & read_raws[5]) >> 7);    // --------------
  
  char year_ones    = '0' + ((0b00001111 & read_raws[6]) >> 0);    // tens ones
  char year_tens    = '0' + ((0b11110000 & read_raws[6]) >> 4);    // ---------
  
  char weakday      = -1  + ((0b00000111 & read_raws[3]) >> 0);
  
  sprintf(formatted_time, "%c%c/%c%c %c%c:%c%c:%c%c %cm",
	  month_tens, month_ones, date_tens, date_ones,
	  hours_tens, hours_ones, minutes_tens, minutes_ones, seconds_tens, seconds_ones,
	  meridian);
  
  float temperature = 1.0f * read_raws[7] + (read_raws[8] >> 6) * 0.25f;
  
  float n_interrupts = (float) schedule -> interrupts;
  
  
  // collected raw values, now to assign output streams
  
  /*bind_stream(ds32, schedule -> interrupts, DS32_MEASURE_TIME       );
    bind_stream(ds32, temperature           , DS32_MEASURE_TEMPERATURE);*/
  
  Output * time_output = &ds32 -> outputs[DS32_MEASURE_TIME];
  Output * temp_output = &ds32 -> outputs[DS32_MEASURE_TEMPERATURE];
  
  time_output -> measure = series_compute(time_output -> series, n_interrupts);
  temp_output -> measure = series_compute(temp_output -> series, temperature );
  
  fprintf(ds32_i2c -> log, "%.4f\t%.2f\t%s\n", time_output -> measure, temp_output -> measure, formatted_time);
  
  sensor_process_triggers(ds32);
  return true;
}

void set_time_ds32(Sensor * ds32) {
  
  uint8 time[7] = {
    0b00000000,    // 0 tens ones           (seconds)
    0b00010110,    // 0 tens ones           (minutes)
    0b00110001,    // 0 am/pm 0 tens ones   (hours  )
    0b00000111,    // 00000 weakday         (weekday)
    0b00010110,    // 00 tens ones          (date   )
    0b00000010,    // century 00 tens ones  (month  )
    0b00011001,    // tens ones             (year   )
  };
  
  i2c_write_bytes(ds32 -> i2c, 0x00, time, 7);
}
