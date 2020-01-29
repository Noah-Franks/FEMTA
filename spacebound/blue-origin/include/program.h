#pragma once

#include "headers.h"

// clock.c (system)
extern  void   real_sleep    (time_t seconds        );
extern  void   milli_sleep   (long ms               );
extern  void   micro_sleep   (long us               );
extern  void   nano_sleep    (long ns               );
extern  long   real_time_diff(struct timespec * past);

// color.c (system)
extern  void   init_color       (void       );
extern  void   drop_color       (void       );
extern  char * get_color_by_name(char * name);

// error.c (system)
extern  void   exit_printing(int code, ...) opt(noreturn);

// file.c (system)
extern  FILE * safe_open(char * path, char * mode);

// gpio.c (system)
extern  void   init_pins         (void                        );
extern  void   drop_pins         (void                        );
extern  void   pin_set           (char broadcom, bool hot     );
extern  void   pin_inform_delays (char broadcom               );
extern  void   pin_set_hot       (void * nil, char * vbroadcom);
extern  void   pin_set_cold      (void * nil, char * vbroadcom);
extern  void   fire              (Charge * charge, bool hot   );
extern  char * recover_from_crash(void                        );

// hashmap.h (structure)
extern  void      key_free          (void * element                         );
extern  int       hash_string       (void *  string, int upper_bound        );    // (char *, int)
extern  int       hash_address      (void * address, int upper_bound        );    // (void *, int)
extern  int       compare_strings   (void * first, void * other             );    // (char *, char *)
extern  int       compare_addresses (void * first, void * other             );    // (void *, void *)
extern  void      print_hashmap_long(HashmapElement * element               );
extern  void    * hashmap_get       (Hashmap * map, void * key              );
extern  void      hashmap_add       (Hashmap * map, void * key, void * value);
extern  void      hashmap_update    (Hashmap * map, void * key, void * value);
extern  bool      hashmap_exists    (Hashmap * map, void * key              );
extern  void      hashmap_remove    (Hashmap * map, void * key              );
extern  void      hashmap_print     (Hashmap * map, element_printer print   );
extern  void      hashmap_delete    (Hashmap * map                          );
extern  Hashmap * hashmap_create    (hash_function hash, key_comparator key_diff,
                                     freer key_free, freer value_free, int size);

// i2c.c (system)
extern  void         init_i2c         (void                                            );
extern  void         drop_i2c         (void                                            );
extern  void         start_i2c        (void                                            );
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
extern  List * list_that_frees  (freer value_free            );  // creates an empty list that frees
extern  void   list_insert      (List * list, void * datum   );  // inserts a node at the end of the list
extern  void   list_insert_first(List * list, void * datum   );  // inserts a node at the beginning of the list
extern  void * list_retrieve    (List * list, int index      );  // gets the value of the node at the index in O(n)
extern  void   list_remove      (List * list, ListNode * node);  // removes a node
extern  void   list_concat      (List * first, List * other  );  // appends the second list to the end of the first
extern  void   list_empty       (List * list                 );  // removes all nodes in the list
extern  void   list_delete      (List * list                 );  // removes all nodes and frees the list itself

// math.c (math)
extern  int    gcd(int a, int b) opt(const);

// memory.c (tests)
extern  void * debug_malloc          (size_t size                             );
extern  void * debug_calloc          (size_t nmemb, size_t size               );
extern  char * debug_strdup          (const char * s                          );
extern  void   debug_free            (void * pointer                          );
extern  FILE * debug_fopen           (const char * pathname, const char * mode);
extern  int    debug_fclose          (FILE * stream                           );
extern  int    debug_yylex_destroy   (void                                    );
extern  void   print_usage_debug_info(void                                    );

// one.c (system)
extern  void         init_one         (void                                                          );
extern  void         drop_one         (void                                                          );
extern  void         start_one        (void                                                          );
extern  one_device * create_one_device(Sensor * sensor, char * path, char * log_path, one_reader read);
extern  void         one_close        (one_device * one                                              );

// parser.l (parser)
extern  int    yylex_destroy(void          );

// parser.y (parser)
extern  void   yyerror      (char * format, ...);
extern  int    yyparse      (void              );
extern  void   print_config (void              );

// sensor.c (sensors)
extern  void  init_sensors           (void                                      );
extern  void  drop_sensors           (void                                      );
extern  void  start_sensors          (void                                      );
extern  float time_passed            (void                                      );
extern  void  bind_stream            (Sensor * sensor, float reading, int stream);
extern  void  sensor_process_triggers(Sensor * sensor                           );
extern  void  sensor_log_header      (Sensor * sensor, char * color             );
extern  void  sensor_log_outputs     (Sensor * sensor, FILE * file              );
extern  void  flip_print             (void * nil, char * raw_text               );

// sensors/*
extern  Sensor * init_ad15(Sensor * proto, char * title, List * modes, List * names);
extern  Sensor * init_adxl(Sensor * proto                                          );
extern  Sensor * init_arm6(Sensor * proto                                          );
extern  Sensor * init_ds18(Sensor * proto, char * path                             );
extern  Sensor * init_ds32(Sensor * proto                                          );
extern  Sensor * init_mcp9(Sensor * proto                                          );
extern  Sensor * init_test(Sensor * proto                                          );

// simulate.c
extern  int   return_zero                        (void                       );
extern  void  print_simulation_info              (void                       );
extern  bool  simulation_read_i2c                (i2c_device * i2c           );
extern  bool  simulation_read_one                (one_device * one           );
extern  int   simulation_gpio_read               (uint8 broadcom             );
extern  int   simulation_gpio_write              (uint8 broadcom, uint8 level);
extern  bool  simulation_report_containment_error(void * nil, ...            ) opt(noreturn);

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
extern  void            calibration_delete             (void * vCalibration                );
extern  Conversion      get_universal_conversion       (char * from, char * to             ) opt(pure);
extern  SeriesElement * series_element_from_conversion (Conversion conversion              );
extern  SeriesElement * series_element_from_calibration(Calibration * calibration          );
extern  void            series_element_delete          (void * vSeriesElement              );
extern  float           series_compute                 (List * series, float x             );
extern  void            series_delete                  (List * series                      );
extern  float           convert_identity               (float x                            ) opt(const);

// user.c (system)
extern  void  present_interface(void);

#ifdef DEBUG_MODE
#define malloc        debug_malloc
#define calloc        debug_calloc
#define strdup        debug_strdup
#define free          debug_free
#define fopen         debug_fopen
#define fclose        debug_fclose
#define yyclean       debug_yylex_destroy
#else
#define yyclean       yylex_destroy          // yylex_destroy is a macro too
#endif

#ifdef SIMULATION_MODE
#define i2cOpen(BUS, ADDR, FLAGS)         return_zero()                          // protect the i2c bus
#define i2cClose(HANDLE)                  return_zero()                          // -------------------
#define i2c_read_byte                     simulation_report_containment_error    // -------------------
#define i2c_read_bytes                    simulation_report_containment_error    // -------------------
#define i2c_raw_read                      simulation_report_containment_error    // -------------------
#define i2c_raw_write                     simulation_report_containment_error    // -------------------
#define i2c_write_byte                    simulation_report_containment_error    // -------------------
#define i2c_write_bytes                   simulation_report_containment_error    // -------------------
#define gpioRead                          simulation_gpio_read                   // redirect actuations
#define gpioWrite                         simulation_gpio_write                  // -------------------
#define gpioSetISRFunc(BCM, EDGE, OUT, F) return_zero();                         // -------------------
#endif
