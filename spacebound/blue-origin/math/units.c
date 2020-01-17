
#include "../include/program.h"

#define take(FROM, TO) convert##_##FROM##_##TO
#define arrow(FROM, TO) #FROM "->" #TO

// temperature
local float take(   C,    K) (float x) { return x + 273.15f;                           }
local float take(   K,    C) (float x) { return x - 273.15f;                           }
local float take(   F,    C) (float x) { return x * (9.0f / 5.0f) + 32.0f;             }
local float take(   C,    F) (float x) { return (x - 32.0f) * (5.0f / 9.0f);           }
local float take(   K,    F) (float x) { return (x - 273.15f) * (9.0f / 5.0f) + 32.0f; }
local float take(   F,    K) (float x) { return (x - 32.0f) * (5.0f / 9.0f) + 273.15f; }

// pressure
local float take( atm,  kPa) (float x) { return x * 101.325f;                          }
local float take( kPa,  atm) (float x) { return x / 101.325f;                          }
local float take( atm, torr) (float x) { return x * 760.0f;                            }
local float take(torr,  atm) (float x) { return x / 760.0f;                            }
local float take( kPa, torr) (float x) { return x * 7.50062f;                          }
local float take(torr,  kPa) (float x) { return x / 7.50062f;                          }

// voltage
local float take(   V,   mV) (float x) { return x * 1000.0f;                           }
local float take(  mV,    V) (float x) { return x / 1000.0f;                           }

// time
local float take(   s,   ms) (float x) { return x * 1000.0f;                           }
local float take(  ms,    s) (float x) { return x / 1000.0f;                           }
local float take(   s,  min) (float x) { return x / 60.0f;                             }
local float take( min,    s) (float x) { return x * 60.0f;                             }
local float take(  ms,  min) (float x) { return x / 60000.0f;                          }
local float take( min,   ms) (float x) { return x * 60000.0f;                          }

local Hashmap * conversions;
local Hashmap * unit_types;
local List    * all_units;

float convert_identity (float x)  { 
  return x;
}

void init_units() {
  
  conversions = hashmap_create(hash_string, compare_strings, NULL, 16);
  unit_types  = hashmap_create(hash_string, compare_strings, NULL, 16);
  
  hashmap_add(conversions, arrow(   C,    K), take(   C,    K));
  hashmap_add(conversions, arrow(   K,    C), take(   K,    C));
  hashmap_add(conversions, arrow(   F,    C), take(   F,    C));
  hashmap_add(conversions, arrow(   C,    F), take(   C,    F));
  hashmap_add(conversions, arrow(   K,    F), take(   K,    F));
  hashmap_add(conversions, arrow(   F,    K), take(   F,    K));
  hashmap_add(conversions, arrow( atm,  kPa), take( atm,  kPa));
  hashmap_add(conversions, arrow( kPa,  atm), take( kPa,  atm));
  hashmap_add(conversions, arrow( atm, torr), take( atm, torr));
  hashmap_add(conversions, arrow(torr,  atm), take(torr,  atm));
  hashmap_add(conversions, arrow( kPa, torr), take( kPa, torr));
  hashmap_add(conversions, arrow(torr,  kPa), take(torr,  kPa));
  hashmap_add(conversions, arrow(   V,   mV), take(   V,   mV));
  hashmap_add(conversions, arrow(  mV,    V), take(  mV,    V));
  hashmap_add(conversions, arrow(   s,   ms), take(   s,   ms));
  hashmap_add(conversions, arrow(  ms,    s), take(  ms,    s));
  hashmap_add(conversions, arrow(   s,  min), take(   s,  min));
  hashmap_add(conversions, arrow( min,    s), take( min,    s));
  hashmap_add(conversions, arrow(  ms,  min), take(  ms,  min));
  hashmap_add(conversions, arrow( min,   ms), take( min,   ms));
  
  hashmap_add(unit_types,    "C", "Temperature");
  hashmap_add(unit_types,    "K", "Temperature");
  hashmap_add(unit_types,    "F", "Temperature");
  hashmap_add(unit_types,  "atm",    "Pressure");
  hashmap_add(unit_types,  "kPa",    "Pressure");
  hashmap_add(unit_types, "torr",    "Pressure");
  hashmap_add(unit_types,    "V",     "Voltage");
  hashmap_add(unit_types,   "mV",     "Voltage");
  hashmap_add(unit_types,    "s",        "Time");
  hashmap_add(unit_types,   "ms",        "Time");
  hashmap_add(unit_types,  "min",        "Time");
  hashmap_add(unit_types,    "i",     "Integer");
  hashmap_add(unit_types,    "f",     "Decimal");
  
  all_units = list_from(12, "raw", "C", "K", "F", "atm", "kPa", "torr", "V", "mV", "s", "ms", "min");
}

void drop_units() {
  hashmap_destroy(conversions);
  hashmap_destroy(unit_types);
  list_destroy(all_units);
}

bool unit_is_supported(char * unit_name) {
  
  for (iterate(all_units, char *, unit))
    if (!strcmp(unit, unit_name))
      return true;
  
  return false;
}

bool unit_is_of_type(Numeric * numeric, char * type) {
  
  char * unit_name = numeric -> units;
  char * unit_type = hashmap_get(unit_types, unit_name);
  
  return !strcmp(unit_type, type);
}

void print_units_supported() {
  printf
    ("Time\n"
     "   s   : system second\n"
     "  ms   : system milli-second\n"
     " min   : system minute\n"
     "\n"
     "Temperature\n"
     "  C    : Celcius\n"
     "  K    : Kelvin\n"
     "  F    : Fahrenheit\n"
     "\n"
     "Pressure\n"
     "  atm  : Atmospheres\n"
     "  kPa  : kilo-Pascals\n"
     "  torr : Torrecelli's unit\n"
     "\n"
     "Voltage\n"
     "   V   : Volts\n"
     "  mV   : milli-Volts\n"
     "\n"
     );
}

Numeric * numeric_from_decimal(float decimal) {
  
  Numeric * numeric = calloc(1, sizeof(*numeric));
  
  numeric -> decimal    = decimal;
  numeric -> is_decimal = true;
  numeric -> units[0]   = 'f';        // generic decimal unit
  
  return numeric;
}

Numeric * numeric_from_integer(float integer) {
  
  Numeric * numeric = calloc(1, sizeof(*numeric));
    
  numeric -> integer    = integer;
  numeric -> is_decimal = false;
  numeric -> units[0]   = 'i';        // generic integer unit
  
  return numeric;
}

SeriesElement * series_element_from_conversion(Conversion conversion) {
  
  SeriesElement * element = calloc(1, sizeof(*element));
  
  element -> universal  = true;
  element -> conversion = conversion;
  
  return element;
}

SeriesElement * series_element_from_calibration(Calibration * calibration) {
  
  SeriesElement * element = calloc(1, sizeof(*element));
  
  element -> universal   = false;
  element -> calibration = calibration;
  
  return element;
}


Conversion get_universal_conversion(char * from, char * to) {
  /* yields the conversion from one unit to the other, assuming the conversion
   * is the same across all domains. In other words, specific conversions related to
   * calibrations on sensors won't be returned; only generic ones like Celcius to Fahrenheit */
  
  if (!strcmp(from, to))
    return convert_identity;
  
  char lookup[16];
  sprintf(lookup, "%s->%s", from, to);
  
  Conversion conversion = hashmap_get(conversions, lookup);
  
  if (unlikely(conversion == NULL)) {
    print_units_supported();
    printf("Unknown conversion %s -> %s\n", from, to);
    yyerror("Please use units from the table above\n");
    gpioTerminate();
    exit(ERROR_EXPERIMENTER);
  }
  
  return conversion;
}

static float compute_curve(float x, Calibration * calibration) {
  
  char * curve     = calibration -> curve;
  List * constants = calibration -> constants;
  
  // a calibration consists of a curve name followed by constants
  
  if (!strcmp(curve, "poly")) {
    
    // Evaluate the polynomial via Horner's Method
    
    Numeric * first = list_get(constants, 0);
    
    float result = first -> decimal;
    
    for (iterate(constants, Numeric *, coefficient)) {
      
      if ((int) coefficient_index < 1) continue;    // skip first node (see list.h)
      
      result = result * x + coefficient -> decimal;
    }
    
    return result;
  }
  
  else if (!strcmp(curve, "hart")) {
    float A = ((Numeric *) list_get(constants, 0)) -> decimal;
    float B = ((Numeric *) list_get(constants, 1)) -> decimal;
    float C = ((Numeric *) list_get(constants, 2)) -> decimal;
    
    float log_x = log(x);    // need to compute Iset
    
    return 1.0f / (A + B * log_x + C * cube(log_x));
  }
  
  printf(RED "Tried to evaluate unknown curve " CYAN "%s\n" RESET);
  exit(ERROR_PROGRAMMER);
}

float series_compute(List * series, float x) {
  // compute a series of conversions to get a final value.
  // the series may include universal conversions as well as sensor-specific ones.
  
  if (!series)    // the null series is defined to be the identity
    return x;
  
  for (iterate(series, SeriesElement *, step))
    if (step -> universal) x = (step -> conversion)(x);
    else                   x = compute_curve(x, step -> calibration);
  
  return x;
}
