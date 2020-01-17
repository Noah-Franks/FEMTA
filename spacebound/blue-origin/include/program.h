#pragma once

#include "headers.h"

// clock.c (system)
extern  void   real_sleep      (time_t seconds        );
extern  void   real_milli_sleep(long ms               );
extern  void   real_nano_sleep (long ns               );
extern  long   real_time_diff  (struct timespec * past);

// color.c (system)
extern  void   init_color       (void       );
extern  void   terminate_color  (void       );
extern  char * get_color_by_name(char * name);

// error.c (system)
extern  void   exit_printing(int code, ...) opt(noreturn);

// file.c (system)
extern  FILE * safe_open(char * path, char * mode);

// gpio.c (system)
extern  void  init_pins        (void                        );
extern  void  pin_set          (char broadcom, bool hot     );
extern  void  pin_inform_delays(char broadcom               );
extern  void  pin_set_hot      (void * nil, char * vbroadcom);
extern  void  pin_set_cold     (void * nil, char * vbroadcom);
extern  void  fire             (Charge * charge, bool hot   );

// hashmap.h (structure)
extern  void      key_free          (void * element                          );
extern  int       hash_string       (void *  string, int upper_bound         );    // (char *, int)
extern  int       hash_address      (void * address, int upper_bound         );    // (void *, int)
extern  int       compare_strings   (void * first, void * other              );    // (char *, char *)
extern  int       compare_addresses (void * first, void * other              );    // (void *, void *)
extern  void      print_hashmap_long(HashmapElement * element                );
extern  void    * hashmap_get       (Hashmap * this, void * key              );
extern  void      hashmap_add       (Hashmap * this, void * key, void * value);
extern  void      hashmap_update    (Hashmap * this, void * key, void * value);
extern  bool      hashmap_exists    (Hashmap * this, void * key              );
extern  void      hashmap_remove    (Hashmap * this, void * key              );
extern  void      hashmap_print     (Hashmap * this, element_printer print   );
extern  void      hashmap_destroy   (Hashmap * this                          );
extern  Hashmap * hashmap_create    (hash_function hash, key_comparator key_diff, 
                                     element_freer freer, int expected_size);

// i2c.c (system)
extern  void         init_i2c         (void                                            );
extern  void         start_i2c        (void                                            );
extern  void         terminate_i2c    (void                                            );
extern  bool         i2c_write_byte   (i2c_device * dev, uint8 reg, uint8 value        );
extern  bool         i2c_write_bytes  (i2c_device * dev, uint8 reg, uint8 * buf, char n);
extern  bool         i2c_read_bytes   (i2c_device * dev, uint8 reg, uint8 * buf, char n);
extern  bool         i2c_raw_write    (i2c_device * dev,            uint8 * buf, char n);
extern  bool         i2c_raw_read     (i2c_device * dev,            uint8 * buf, char n);
extern  uint8        i2c_read_byte    (i2c_device * dev, uint8 reg                     );
extern  void         i2c_close        (i2c_device * i2c                                );
extern  i2c_device * create_i2c_device(Sensor * sensor, i2c_reader reader              );

// list.c (structure)
extern  List * list_create      (void                        );  // creates an empty list
extern  List * list_from        (int vargs, ...              );  // creates a list pre-filled with elements
extern  List * list_that_frees  (ValueFree freer             );  // creates an empty list that frees
extern  void   list_insert      (List * list, void * datum   );  // inserts a node at the end of the list
extern  void   list_insert_first(List * list, void * datum   );  // inserts a node at the beginning of the list
extern  void   list_remove      (List * list, ListNode * node);  // removes a node
extern  void   list_concat      (List * first, List * other  );  // appends the second list to the end of the first
extern  void   list_lock        (List * list                 );  // locks a parallel list
extern  void   list_unlock      (List * list                 );  // unlocks a parallel list
extern  void   list_empty       (List * list                 );  // removes all nodes in the list
extern  void   list_destroy     (List * list                 );  // removes all nodes and frees the list itself

// math.c (math)
extern  int    gcd(int a, int b) opt(const);

// one.c (system)
extern  void         init_one         (void                                                          );
extern  void         start_one        (void                                                          );
extern  void         terminate_one    (void                                                          );
extern  one_device * create_one_device(Sensor * sensor, char * path, char * log_path, one_reader read);
extern  void         one_close        (one_device * one                                              );

// parser.y (parser)
extern  void   yyerror     (char * message);
extern  int    yyparse     (void          );
extern  void   print_config(void          );

// selector.c (structure)
extern  Selector * create_selector     (Selector * parent                             );
extern  void       execute_selector    (Selector * selector, char key, char * raw_text);
extern  void       output_str          (void * nil, char * raw_text);
extern  void       flip_bool           (void * pointer, char * raw_text);
extern  void       add_selector_command(Selector * selector,
                       char key, char * text, selector_action action, void * argument );

// sensor.c (sensors)
extern  float time_passed            (void                         );
extern  void  init_sensors           (void                         );
extern  void  start_sensors          (void                         );
extern  void  terminate_sensors      (void                         );
extern  void  sensor_process_triggers(Sensor * sensor              );
extern  void  sensor_log_header      (Sensor * sensor, char * color);
extern  void  sensor_log_outputs     (Sensor * sensor, FILE * file );
extern  void  sensor_print_to_console(Sensor * sensor              );
extern  void  flip_print             (void * nil, char * raw_text  );

// sensors/*
extern  Sensor * init_ad15(Sensor * proto, char * title, List * modes, List * names);
extern  Sensor * init_adxl(Sensor * proto                                          );
extern  Sensor * init_ds18(Sensor * proto, char * path                             );
extern  Sensor * init_ds32(Sensor * proto                                          );
extern  Sensor * init_mcp9(Sensor * proto                                          );
extern  Sensor * init_test(Sensor * proto                                          );

// state.c (system)
extern  void         init_states        (void                      );
extern  void         drop_states        (void                      );
extern  void         print_all_states   (void                      );
extern  void         add_state          (char * state, bool entered);
extern  bool         state_exists       (char * state              );
extern  void         enter_state        (char * state              );
extern  void         leave_state        (char * state              );
extern  void         enter              (Transition * trans        );
extern  void         leave              (Transition * trans        );
extern  bool         state_get          (char * state              );
extern  Transition * transition_create  (char * state, int delay   );
extern  void         state_inform_delays(char * state              );

// units.c (math)
extern  void            init_units                     (void                               );
extern  void            drop_units                     (void                               );
extern  void            print_units_supported          (void                               );
extern  bool            unit_is_supported              (char * unit_name                   ) opt(pure);
extern  bool            unit_is_supported              (char * unit_name                   ) opt(pure);
extern  bool            unit_is_of_type                (Numeric * numeric, char * type_name) opt(pure);
extern  Numeric *       numeric_from_decimal           (float decimal                      );
extern  Numeric *       numeric_from_integer           (float integer                      );
extern  Conversion      get_universal_conversion       (char * from, char * to             ) opt(pure);
extern  SeriesElement * series_element_from_conversion (Conversion conversion              );
extern  SeriesElement * series_element_from_calibration(Calibration * calibration          );
extern  float           series_compute                 (List * series, float x             );
extern  void            series_destroy                 (List * series                      );
extern  float           convert_identity               (float x                            ) opt(const);
