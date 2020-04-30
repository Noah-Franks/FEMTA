#pragma once

#include "headers.h"

// clock.c (system)
extern  void   micro_sleep   (long us               );    // sleep a number of microseconds
extern  void   nano_sleep    (long ns               );    // sleep a number of nanoseconds
extern  long   real_time_diff(struct timespec * past);    // get nanoseconds since a point in the past

// console.c (system)
extern  void   init_console     (void       );    // set up the color system
extern  void   drop_console     (void       );    // let go of the color system
extern  char * get_color_by_name(char * name);    // takes names to bash color codes

// error.c (system)
extern  void   exit_printing(int code, ...     ) opt(noreturn);    // safely exit after printing
extern  void   print_error  (char * format, ...);                  // print an error message

// file.c (system)
extern  FILE * safe_open(char * path, char * mode       );    // ensure a file can actually be opened
extern  bool   scan_file(char * path, char * format, ...);    // dump a (best virtual) file into args

// gpio.c (system)
extern  void   process_pin_queue (int duration                );    // process delayed actuations
extern  void   pin_set           (char gpio, bool hot         );    // set a pin hot or cold
extern  void   pin_queue         (PinChange * change, bool hot);    // set a complex pin change
extern  char * recover_from_crash(void                        );    // run this in gdb after crash

// hashmap.c (structure)
extern  int       hash_string       (void *  string, u32 upper_bound        );     // hash any string
extern  int       hash_address      (void * address, u32 upper_bound        );     // hash any address
extern  int       compare_strings   (void * first, void * other             );     // see if strings are equal
extern  int       compare_addresses (void * first, void * other             );     // see if addresses are equal
extern  void      hashmap_add       (Hashmap * map, void * key, void * value);     // insert element into map
extern  void    * hashmap_get       (Hashmap * map, void * key              );     // retrieve element from map
extern  void      hashmap_update    (Hashmap * map, void * key, void * value);     // update an existing element
extern  bool      hashmap_exists    (Hashmap * map, void * key              );     // see if element exists in map
extern  void    * hashmap_delete    (Hashmap * map                          );     // completely delete the map
extern  void      vhashmap_delete   (Hashmap * map                          );     // same, but void return type
extern  Hashmap * hashmap_create    (hash_function hash, key_comparator key_diff,  // make a new map
                                     freer key_free, freer value_free, int size);

// i2c.c (system)
extern  void         init_i2c         (void                                              );
extern  void         drop_i2c         (void                                              );
extern  void         start_i2c        (void                                              );
extern  bool         i2c_write_byte   (i2c_device * dev, uint8 reg, uint8 value          );
extern  bool         i2c_write_bytes  (i2c_device * dev, uint8 reg, uint8 * buf, char n  );
extern  bool         i2c_read_bytes   (i2c_device * dev, uint8 reg, uint8 * buf, char n  );
extern  bool         i2c_raw_write    (i2c_device * dev,            uint8 * buf, char n  );
extern  bool         i2c_raw_read     (i2c_device * dev,            uint8 * buf, char n  );
extern  uint8        i2c_read_byte    (i2c_device * dev, uint8 reg                       );
extern  void         i2c_close        (i2c_device * i2c                                  );
extern  i2c_device * create_i2c_device(Sensor * sensor, i2c_reader reader, char * message);

// list.c (structure)
extern  List * list_create      (void                        );  // creates an empty list
extern  List * list_from        (int vargs, ...              );  // creates a list holding the arguments
extern  List * list_that_frees  (freer value_free            );  // creates an empty list that frees its elements
extern  void   list_insert      (List * list, void * datum   );  // inserts a node at the end of the list
extern  void   list_insert_first(List * list, void * datum   );  // inserts a node at the beginning of the list
extern  void * list_retrieve    (List * list, int index      );  // retrieves the ith element in the list in O(n)
extern  void   list_remove      (List * list, ListNode * node);  // removes a node, freeing if necessary
extern  void * list_delete      (List * list                 );  // completely deletes the list and its elements

// math.c (math)
extern  int    gcd(int a, int b) opt(const);    // compute the greatest common factor
extern  int    lcm(int a, int b) opt(const);    // compute the least common multiple

// memory.c (tests)
extern  void * debug_malloc          (size_t size                             );  // debug versions
extern  void * debug_calloc          (size_t nmemb, size_t size               );  // --------------
extern  void * debug_realloc         (void * pointer, size_t size             );  // --------------
extern  char * debug_strdup          (const char * s                          );  // --------------
extern  void   debug_free            (void * pointer                          );  // --------------
extern  FILE * debug_fopen           (const char * pathname, const char * mode);  // --------------
extern  int    debug_fclose          (FILE * stream                           );  // --------------
extern  int    debug_yylex_destroy   (void                                    );  // --------------
extern  void   print_usage_debug_info(void                                    );  // print stats to console

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

// plot.c (tests)
extern  void  consider_plotting_sensors(int duration);
extern  void  plot_fgets               (char * s, int n, FILE * stream);

// schedule.c (system)
extern  float time_passed              (void        );
extern  void  build_schedule           (void        );
extern  void  consider_printing_sensors(int duration);

// sensor.c (sensors)
extern  void  init_sensors           (void                                         );
extern  void  drop_sensors           (void                                         );
extern  void  bind_stream            (Sensor * sensor, float reading, int stream   );
extern  void  sensor_process_triggers(Sensor * sensor                              );
extern  void  sensor_log_header      (Sensor * sensor, char * color                );
extern  void  sensor_log_outputs     (Sensor * sensor, FILE * file, char * addendum);
extern  void  trigger_delete         (void * vtrigger                              );
extern  void  flip_print             (void * nil, char * raw_text                  );

// sensors/*
extern  Sensor * init_ad15(Sensor * proto, List * modes);
extern  Sensor * init_adxl(Sensor * proto              );
extern  Sensor * init_arm6(Sensor * proto              );
extern  Sensor * init_ds18(Sensor * proto, char * path );
extern  Sensor * init_ds32(Sensor * proto              );
extern  Sensor * init_mcp9(Sensor * proto              );
extern  Sensor * init_slf3(Sensor * proto              );
extern  Sensor * init_test(Sensor * proto              );
extern  Sensor * init_veml(Sensor * proto              );

// simulate.c
extern  int   return_zero                        (void                       );
extern  void  print_simulation_info              (void                       );
extern  bool  simulation_read_i2c                (i2c_device * i2c           );
extern  bool  simulation_read_one                (one_device * one           );
extern  int   simulation_gpio_read               (uint8 broadcom             );
extern  int   simulation_gpio_write              (uint8 broadcom, uint8 level);
extern  bool  simulation_report_containment_error(void * nil, ...            ) opt(noreturn);

// state.c (system)
extern  void    drop_states        (void                            );    // remove all states
extern  State * state_create       (char * name,          bool enter);    // track a new state
extern  State * state_get          (char * name                     );    // get a state, if it exists O(n)
extern  void    state_set          (State * state,        bool enter);    // set a state
extern  void    state_queue        (StateChange * change, bool enter);    // queue a state change
extern  void    process_state_queue(int duration                    );    // process delayed changes
extern  void    wait_on_mission    (void                            );    // wait for mission complete
extern  void    print_all_states   (void                            );    // nicely print every state
  
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
extern  void  present_interface(void);    // allow the user to command the process

#ifdef DEBUG_MODE
#define malloc        debug_malloc
#define calloc        debug_calloc
#define realloc       debug_realloc
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

#ifdef PLOT_MODE
#define consider_printing_sensors consider_plotting_sensors
#endif
