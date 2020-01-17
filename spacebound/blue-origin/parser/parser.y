
%{
#include "../include/program.h"

extern FILE * yyin;

int  yylex();
void yyerror(char * message);

typedef struct EffectNode EffectNode;
typedef struct Specification Specification;

EffectNode    * make_charge(Numeric * wire, bool hot);
EffectNode    * make_transition(char * state_name, bool entering);
EffectNode    * add_delay(EffectNode * effect, Numeric * delay);
Trigger       * make_trigger(List * effects);
Specification * extend_trigger(List * state_names, char * id, bool less, Numeric * threshold, List * options, Trigger * trigger);
Specification * make_tag(char * id, List * options, List * args);

void check_and_add_state(char * name, bool entered);
void build_sensor(char * id, Numeric * frequency, Numeric * denominator, List * specifications);
void print_config();
%}

%code requires {
  
  #include "../include/program.h"
  
  typedef struct EffectNode {
    
    bool  is_charge;    // whether effect is a charge or transition
    float delay_ms;
    
    union {
      struct {
          Charge * charge;            // charge of a wire state to another
          bool     hot;
      };
      
      struct {
          Transition * transition;    // transition from one state to another
          bool         entering;
      };
    };
    
  } EffectNode;
  
  typedef struct Specification {
      
      char * id;
      List * options;
      List * args;
    
  } Specification;
}

%union {
  char          * string;
  int           * integer;
  float         * decimal;
  Numeric       * numeric;
  
  List          * list;
  EffectNode    * effect;
  Trigger       * trigger;
  Specification * specification;
}

%token IF SET ENTER LEAVE AFTER STATE PIN POS NEG DEFINE SENSOR

%token  <string>        ID
%token  <numeric>       NUMERIC
                        
%type   <list>          Specs Args Options Effects
%type   <trigger>       Actuator
%type   <effect>        Effect
%type   <specification> Spec Tag
                        
%start Config

                        /* Notes:                                             *
                         *                                                    *
                         * Specifications include triggers and tags.          *
                         * Tags may be destroyed when their specification is  *
                         * interpreted, as is the case whith make_charge()    *
                         *                                                    *
                         * Units must never exceed 7 characters.              *
                         * Additionally, two generic units exist: i and f     *
                         *   i - the generic 32-bit integer                   *
                         *   f - the generic 32-bit floating point number     *
                         *                                                    *
                         */

%%

Config   : Defs Sensors
         | Defs                                            { printf("\nExperiment does not use sensors");         }
         ;

Defs     : Def
         | Defs Def
         ;

Def      : DEFINE LEAVE ID                                 { check_and_add_state($3, false);                      }
         | DEFINE ENTER ID                                 { check_and_add_state($3,  true);                      }
         ;

Sensors  : Sensor                
         | Sensors Sensor
         ;

Sensor   : SENSOR ID NUMERIC             '{'       '}'     { build_sensor($2, $3,      NULL,  NULL);              }
         | SENSOR ID NUMERIC             '{' Specs '}'     { build_sensor($2, $3,      NULL,    $5);              }
         | SENSOR ID NUMERIC '/' NUMERIC '{'       '}'     { build_sensor($2, $3, $5, NULL);                      }
         | SENSOR ID NUMERIC '/' NUMERIC '{' Specs '}'     { build_sensor($2, $3, $5,   $7);                      }
         ;

Specs    : Spec                                            { $$ = list_from(1, $1);                               }
         | Specs Spec                                      { list_insert($1, $2); $$ = $1;                        }
         ;

Spec     : IF '(' ID '<' NUMERIC             ')' Actuator  { $$ = extend_trigger(NULL, $3,  true, $5, NULL, $7);  }
         | IF '(' ID '<' NUMERIC ':' Options ')' Actuator  { $$ = extend_trigger(NULL, $3,  true, $5,   $7, $9);  }
         | IF '(' ID '>' NUMERIC             ')' Actuator  { $$ = extend_trigger(NULL, $3, false, $5, NULL, $7);  }
         | IF '(' ID '>' NUMERIC ':' Options ')' Actuator  { $$ = extend_trigger(NULL, $3, false, $5,   $7, $9);  }

         | IF '(' STATE Options ':' ID '<' NUMERIC             ')' Actuator
    	      { $$ = extend_trigger(  $4, $6,  true, $8, NULL, $10); }

         | IF '(' STATE Options ':' ID '<' NUMERIC ':' Options ')' Actuator
	      { $$ = extend_trigger(  $4, $6,  true, $8,  $10, $12); }

         | IF '(' STATE Options ':' ID '>' NUMERIC             ')' Actuator
	      { $$ = extend_trigger(  $4, $6, false, $8, NULL, $10); }

         | IF '(' STATE Options ':' ID '>' NUMERIC ':' Options ')' Actuator
	      { $$ = extend_trigger(  $4, $6, false, $8,  $10, $12); }

         | Tag                                             { $$ = $1;                                             }
         ;

Actuator : '{'         '}'                                 { yyerror("Empty if body");                            }
         | '{' Effects '}'                                 { $$ = make_trigger($2);                               }
         ;

Effects  : Effect                                          { $$ = list_from(1, $1);                               }
         | Effect AFTER NUMERIC                            { $$ = list_from(1, add_delay($1, $3));                }
         | Effects Effect                                  { list_insert($1, $2); $$ = $1;                        }
         | Effects Effect AFTER NUMERIC                    { list_insert($1, add_delay($2, $4)); $$ = $1;         }
         ;

Effect   : SET PIN NUMERIC POS                             { $$ = make_charge($3,  true);                         }
         | SET PIN NUMERIC NEG                             { $$ = make_charge($3, false);                         }
         | ENTER ID                                        { $$ = make_transition($2,  true);                     }
         | LEAVE ID                                        { $$ = make_transition($2, false);                     }
         ;

Tag      : '[' ID                      ']'                 { $$ = make_tag($2, NULL, NULL);                       }
         | '[' ID ':'         ':' Args ']'                 { $$ = make_tag($2, NULL,   $5);                       }
         | '[' ID ':' Options ':'      ']'                 { $$ = make_tag($2,   $4, NULL);                       }
         | '[' ID ':' Options ':' Args ']'                 { $$ = make_tag($2,   $4,   $6);                       }
         ;

Args     : Args ',' NUMERIC                                { list_insert($1, $3); $$ = $1;                        }
         | NUMERIC                                         { $$ = list_from(1, $1);                               }
         ;

Options  : ID                                              { $$ = list_from(1, $1);                               }
         | Options ',' ID                                  { list_insert($1, $3); $$ = $1;                        }
         ;

%%

static void check_unit(char * unit_name) {
    if (!unit_is_supported(unit_name)) {
      printf(RED "Unknown unit " CYAN "%s\n" RESET, unit_name);
      yyerror("Unknown unit");
    }
}

static void check_target(Sensor * proto, char * target) {
    if (!proto -> targets || !hashmap_exists(proto -> targets, target)) {
        printf(RED "Target " CYAN "%s " RED "for %s is not known\n" RESET, target, proto -> code_name);
        yyerror("Unknown target");
    }
}

static void represent_as_decimal(Numeric * numeric) {
    
    if (numeric -> is_decimal) return;
    
    numeric -> decimal    = (float) numeric -> integer;
    numeric -> is_decimal = true;
    
    if (!strcmp(numeric -> units, "i"))    // change generic unit if needed
      numeric -> units[0] = 'f';           // -----------------------------
}

static void represent_as_integer(Numeric * numeric) {
    
    if (!numeric -> is_decimal) return;
    
    numeric -> integer    = (int) numeric -> decimal;
    numeric -> is_decimal = false;
    
    if (!strcmp(numeric -> units, "f"))      // change generic unit if needed
        numeric -> units[0] = 'i';           // -----------------------------
}

static void specification_destroy(void * vspecification) {
    
    Specification * specification = vspecification;
        
    free(specification -> id);
    free(specification);
}

void check_and_add_state(char * name, bool entered) {
  
  if (state_exists(name)) {
      printf(RED "State " CYAN "%s " RED "already defined\n" RESET, name);
      yyerror("state already defined");
  }
  
  add_state(name, entered);
}

EffectNode * make_charge(Numeric * wire, bool hot) {
    
    if (!unit_is_of_type(wire, "Integer"))
	yyerror("Wires must be integers");
    
    if (!between(0, wire -> integer, 27)) {
        printf(RED "Broadcom " CYAN "%d " RED "does not exist\n" RESET, wire -> integer);
        yyerror("Nonexistant broadcom specified");
    }
    
    Charge * charge = calloc(1, sizeof(*charge));
    
    charge -> gpio = wire -> integer;    // delays are added later
    
    EffectNode * effect = calloc(1, sizeof(*effect));

    effect -> charge = charge;
    effect -> hot    = hot;
    
    effect -> is_charge = true;    
    return effect;
}

EffectNode * make_transition(char * state_name, bool entering) {
  
    if (!state_exists(state_name)) {
        printf(RED "Unknown state name: " CYAN "%s\n" RESET, state_name);
        yyerror("State not found");
    }
  
    EffectNode * effect  = calloc(1, sizeof(*effect));
  
    effect -> is_charge  = false;
    effect -> transition = transition_create(state_name, 0);    // delays are added later
    effect -> entering   = entering;
  
    return effect;
}

EffectNode * add_delay(EffectNode * effect, Numeric * delay) {
  
    represent_as_decimal(delay);
  
    if (delay -> decimal <= 0) {
        printf(RED "Delay %f not allowed: value is non-positive" RESET, delay -> decimal);
        yyerror("Improper delay specified");
    }
  
    if (!unit_is_supported(delay -> units) || !unit_is_of_type(delay, "Time")) {
        printf("%s is not a unit of time", delay -> units);
        yyerror("Improper delay specified");
    }
  
    effect -> delay_ms = (get_universal_conversion(delay -> units, "ms"))(delay -> decimal);
  
    if (effect -> is_charge) effect -> charge     -> delay = (int) effect -> delay_ms;
    else                     effect -> transition -> delay = (int) effect -> delay_ms;
  
    if (effect -> is_charge) pin_inform_delays(effect -> charge -> gpio);
    else                     state_inform_delays(effect -> transition -> state);
  
    return effect;
}

Trigger * make_trigger(List * effects) {
    // creates a trigger, which becomes a specification later
    
    Trigger * trigger = calloc(1, sizeof(*trigger));
    
    trigger -> fired    = false;
    trigger -> singular =  true;    // defaults
    trigger -> reverses = false;    // --------
    
    trigger -> wires_low  = list_create();
    trigger -> wires_high = list_create();
    trigger -> enter_set  = list_create();
    trigger -> leave_set  = list_create();
    
    for (iterate(effects, EffectNode *, effect))  
        if (effect -> is_charge)
            if (effect -> hot     ) list_insert(trigger -> wires_high, effect -> charge    );
            else                    list_insert(trigger -> wires_low , effect -> charge    );
        else
            if (effect -> entering) list_insert(trigger -> enter_set , effect -> transition);
            else                    list_insert(trigger -> leave_set , effect -> transition);
    
    list_destroy(effects);
    return trigger;
}


Specification * extend_trigger(
                               List    * state_names,    // what conjunction precondition this trigger
                               char    * id,             // the output stream name (like A0)
                               bool      less,           // the comparison direction with the threshold
                               Numeric * threshold,      // the threshold beyond which actuation occurs
                               List    * options,        // the specific type of trigger
                               Trigger * trigger         // the trigger to be extended
                               ) {
    /* Wraps and modifies the trigger into a specification so that sensor construction *
     * may have a linked list consisting of nodes with the same content                */

    if (state_names) {
        for (iterate(state_names, char *, state_name)) {
            if (!state_exists(state_name)) {
                printf(RED "Unknown state name " CYAN "%s\n" RESET, state_name);
                yyerror("Invalid state list for trigger");
            }
        }
	
        trigger -> precondition = state_names;
    } else {
        trigger -> precondition = list_create();
    }
    

    
    
    represent_as_decimal(threshold);
    
    trigger -> id   = id;
    trigger -> less = less;
    
    trigger -> threshold_as_specified = threshold;    // this gets converted later
    
    if (options) {
        for (iterate(options, char *, option)) {
            if      (!strcmp(option, "single"  )) trigger -> singular =  true;
            else if (!strcmp(option, "forever" )) trigger -> singular = false;
            else if (!strcmp(option, "reverses")) trigger -> reverses =  true;
            else {
                printf(RED "Unknown option " CYAN "%s\n" RESET, option);
                yyerror("Unknown option specified");
            }
        }
    }
    
    Specification * specification = calloc(1, sizeof(*specification));
    
    specification -> id      = strdup("trigger");
    specification -> options = list_from(1, trigger);
    
    return specification;
}

Specification * make_tag(char * id, List * options, List * args) {
  
    Specification * tag = calloc(1, sizeof(*tag));
    
    tag -> id      = id;
    tag -> options = options;
    tag -> args    = args;
        
    
    /* Perform preliminary error checking on this tag now */
    
    if (!strcmp(id, "print")) {
      
        if (options) {
	  
            if (options -> size > 1) yyerror("Printing permits at most 1 option (the console color)");
	    
            char * color = list_get(options, 0);
	    
            if (!get_color_by_name(color)) {
                printf(RED "Color name %s not recognized\n" RESET, color);
                yyerror("Invalid color name specified");
            }
        }
	
        if (args) {
	  
            Numeric * numeric = list_get(args, 0);
	  
            if (args -> size > 1               ) yyerror("Printing permits at most 1 argument (the frequency)");
            if (!strcmp(numeric -> units, "Hz")) yyerror("Frequencies must be in Hz"                          );
            if (numeric -> is_decimal          ) yyerror("Frequencies must be integers or rational numbers"   );
        }
    }
        
    else if (!strcmp(id, "smooth")) {
      
        if (!options || options -> size != 1) yyerror("Smoothing requires exactly 1 argument (the target)");
	
        if (args) {
            Numeric * numeric = list_get(args, 0);
            float value = numeric -> decimal;
	    
            if (strcmp(numeric -> units, "f") || value < 0.0f || value > 1.0f)
                yyerror("Autoregressive smoothing requires a decimal # in [0.0, 1.0]");
        }
    }
    
    else if (!strcmp(id, "calibrate")) {
        
        if (!args   )            yyerror("Calibration curves require at least one constant");
        if (!options)            yyerror("Calibration requires a target");
        if (options -> size < 4) yyerror("Calibration requires a unit 'from' dfollowed by a unit 'to'");
        
        char * curve = (char *) list_get(options, 1);
        
        if      (!strcmp(curve, "poly"));
        else if (!strcmp(curve, "hart")) {
            if (args -> size != 3)
                yyerror("The Steinhart and Hart Equation requires exactly 3 constants");
        }
        else {
            printf(RED "Unknown calibration curve " CYAN "%s\n" RESET, curve);
            yyerror("Malformed calibration");
        }
        
        char * unit_from = list_get(options, 2);
        char * unit_to   = list_get(options, 3);
        
        check_unit(unit_from);
        check_unit(unit_to);
    }
    
    else if (!strcmp(id, "conversions")) {
      
        if (args                               ) yyerror("A conversion series should never have arguments"         );
        if (!options || options -> size < 3    ) yyerror("A conversion series must include a target, then 2+ units");
        if (strcmp(list_get(options, 1), "raw")) yyerror("A conversion series always starts with 'raw'"            );
      
        for (iterate(options, char *, unit_name)) {
	
            if ((int) unit_name_index < 1) continue;    // first is target, so skip (see list.h)
	    
            check_unit(unit_name);
        }
    }
    
    else {
        printf(RED "Invalid tag id " CYAN "%s\n" RESET, id);
        yyerror("Malformed tag");
    }
    
    return tag;
}

void build_sensor(char * id, Numeric * frequency, Numeric * denominator, List * specifications) {
  
    Sensor * proto = hashmap_get(all_sensors, id);
  
    if (!proto) {
        printf(CYAN "%s " RED "is not a sensor\n" RESET, id);
        yyerror("Unknown sensor name");
    }
  
    if ((               strcmp(frequency   -> units, "Hz") && strcmp(frequency   -> units, "i")) ||
        (denominator && strcmp(denominator -> units, "Hz") && strcmp(denominator -> units, "i")))
        yyerror("Sensor frequencies must be integers or rational numbers of Hz");
  
    proto -> hertz = frequency -> integer;    // base Hz for scheduling
  
    if (denominator) proto -> hertz_denominator = denominator -> integer;
    else             proto -> hertz_denominator = 0;
  
    proto -> requested = true;
  
    for (int stream = 0; stream < proto -> data_streams; stream++)
        proto -> outputs[stream].triggers = list_create();
  
    if (!specifications) return;
  
    // TODO: break this off into a per-target curves
    List * all_calibrations = list_create();
  
    for (iterate(specifications, Specification *, specification)) {
      
        List * options = specification -> options;
        List * args    = specification -> args;
      
        if (!strcmp(specification -> id, "trigger")) {
          
            /* can't yet do conversion, reversals, etc,
             * so we do the rest in a later for loop */
          
            Trigger * trigger = list_get(options, 0);    // we encapsulate the trigger within the options list
          
            check_target(proto, trigger -> id);
          
            int stream = (int) hashmap_get(proto -> targets, trigger -> id);
          
            list_insert(proto -> outputs[stream].triggers, trigger);
            list_destroy(options);
        }
      
        else if (!strcmp(specification -> id, "print")) {
          
            if (options) {
                proto -> print_code = get_color_by_name(list_get(options, 0));
                list_destroy(options);
            }
          
            if (args) {
                proto -> print_hertz = ((Numeric *) list_get(args, 0)) -> integer;
                list_destroy(args);
            }
          
            proto -> print = true;
        }
      
        else if (!strcmp(specification -> id, "smooth")) {
          
            Numeric * numeric = list_get(args, 0);
            char    * target  = list_get(options, 0);
          
            float regressive = numeric -> decimal;
          
            check_target(proto, target);
          
            int stream = (int) hashmap_get(proto -> targets, target);
          
            if (proto -> outputs[stream].regressive != 1.0f) {
                printf(RED "Found second autoregressive constant for %s\n" RESET, id);
                yyerror("More than one autoregressive constant");
            }
          
            proto -> outputs[stream].regressive = regressive;
            list_destroy(options);
            list_destroy(args);
        }
      
        else if (!strcmp(specification -> id, "calibrate")) {
          
            /* as with the triggers, we can't do everything here (like forming each output series)
             * so we'll place each calibration (which are themselves lists) into an UNORDERED list
             * for each target using the calibrations hashmap. Later, we'll actually convert these
             * unordered lists into proper serieses. */
          
            char * target    = list_get(options, 0);
            char * curve     = list_get(options, 1);
            char * unit_from = list_get(options, 2);
            char * unit_to   = list_get(options, 3);
          
            check_target(proto, target);
          
            // force all calibration constants to be decimals
            for (iterate(specification -> args, Numeric *, constant))
                represent_as_decimal(constant);
          
          
            Calibration * calibration = calloc(1, sizeof(*calibration));
          
            calibration -> curve     = curve;
            calibration -> constants = specification -> args;
            calibration -> unit_from = unit_from;
            calibration -> unit_to   = unit_to;
            calibration -> target    = target;
          
            list_insert(all_calibrations, calibration);
        }
      
        else if (!strcmp(specification -> id, "conversions")) {
	  
            char * target = list_get(options, 0);
          
            check_target(proto, target);
          
            int stream = (int) hashmap_get(proto -> targets, target);
          
            if (proto -> outputs[stream].series) {
                printf(RED "Found second conversion series for %s's " CYAN "%s\n", id, target);
                yyerror("More than one conversion series for target exists");
            }
          
            /* Temporarily store the options list. This gets replaced
             * with the actual series when iterating over the calibrations */
            list_remove(options, options -> head);        // remove the target so only unit names exist
            proto -> outputs[stream].series = options;
        }
    }
  
  
    for (int stream = 0; stream < proto -> data_streams; stream++) {
        
        char * output_name = proto -> outputs[stream].name;
        
        List * unit_names = proto -> outputs[stream].series;    // get the temporarily stored unit names
        
        // make sure in the event no series was specified that there's no calibrations
        if (!unit_names) {
            for (iterate(all_calibrations, Calibration *, calibration)) {
                if (stream == (int) hashmap_get(proto -> targets, calibration -> target)) {
                    printf("Found calibration without conversions series for %s\n", calibration -> target);
                    yyerror("Calibration without proper conversion series context");
                }
            }
            continue;    // no unit names means no conversion series
        }
        
        List * series = list_create();
        
        char * last_name = list_get(unit_names, 0);
        
        for (iterate(unit_names, char *, name)) {
            
            if (!strcmp(last_name, name)) continue;    // nothing need be done when same
            
            // see if sensor has specified a calibration for this unit
            
            SeriesElement * element = NULL;
            
            for (iterate(all_calibrations, Calibration *, calibration)) {
                if (strcmp(output_name, calibration -> target   )) continue;
                if (strcmp(  last_name, calibration -> unit_from)) continue;
                if (strcmp(       name, calibration -> unit_to  )) continue;
                
                element = series_element_from_calibration(calibration);
            }
            
            if (!element)    // does error checking
                element = series_element_from_conversion(get_universal_conversion(last_name, name));
            
            list_insert(series, element);
            last_name = name;
        }
        
        proto -> outputs[stream].unit   = last_name;
        proto -> outputs[stream].series = series;
        list_destroy(unit_names);
    }
    
    list_destroy(all_calibrations);
    all_calibrations = NULL;
    
    
    for (int stream = 0; stream < proto -> data_streams; stream++) {
      
        if (!proto -> outputs[stream].unit)
            proto -> outputs[stream].unit = "raw";
      
        char * final_unit = proto -> outputs[stream].unit;
        List * triggers   = proto -> outputs[stream].triggers;
      
        // perform conversions
        for (iterate(triggers, Trigger *, trigger)) {
	  
            Numeric * before = trigger -> threshold_as_specified;
	  
            trigger -> threshold = (get_universal_conversion(before -> units, final_unit))(before -> decimal);
        }
      
        // duplicate reversing triggers
        for (iterate(triggers, Trigger *, trigger)) {
	
            if (!trigger -> reverses) continue;
	  
            Trigger * opposite = calloc(1, sizeof(*opposite));
	  
            opposite -> id       =  strdup(trigger -> id);
            opposite -> less     = !trigger -> less;
            opposite -> fired    =  trigger -> fired;
            opposite -> singular =  trigger -> singular;
            opposite -> reverses =  false;
	  
            opposite -> threshold              = trigger -> threshold;
            opposite -> threshold_as_specified = trigger -> threshold_as_specified;
            opposite -> precondition           = trigger -> precondition;
            opposite -> enter_set              = trigger -> enter_set;
            opposite -> leave_set              = trigger -> leave_set;
	  
            opposite -> wires_low  = trigger -> wires_high;    // note the reversal!
            opposite -> wires_high = trigger -> wires_low;     // ------------------
	  
            list_insert(triggers, opposite);
        }

        n_triggers += triggers -> size;
    }
  
    specifications -> free = specification_destroy;    // Note: up to us to destroy lists earlier
    list_destroy(specifications);                      // ---------------------------------------
}

void print_config() {
  
    if (!n_triggers) {
        printf("\n\nExperiment does not involve triggers\n\n");
        return;
    }

    printf("\n\n");
  
    print_all_states();
  
    //printf(GRAY "%d" RESET " Triggers\n\n", n_triggers);
  
    for (iterate(all_sensors -> all, Sensor *, proto)) {
    
        if (!proto -> requested) continue;
    
        int total_triggers = 0;
        for (int stream = 0; stream < proto -> data_streams; stream++)
            total_triggers += proto -> outputs[stream].triggers -> size;
    
        if (!total_triggers) continue;
    
        printf(GREEN "%s\n" RESET, proto -> code_name);
    
        for (int stream = 0; stream < proto -> data_streams; stream++) {
      
            char * final_unit = proto -> outputs[stream].unit;
      
            for (iterate(proto -> outputs[stream].triggers, Trigger *, trigger)) {
	
                Numeric * before = trigger -> threshold_as_specified;
                char   direction = (trigger -> less) ? '<' : '>';
	
                printf(CYAN "    %s" GRAY " %c" MAGENTA " %.3f%s " GRAY "(= " MAGENTA "%.3f%s" GRAY ")",
                       trigger -> id, direction, before -> decimal, before -> units, trigger -> threshold, final_unit);
	
                if (trigger -> precondition -> size) {
	  
                    printf(" in { " BLUE);
	  
                    for (iterate(trigger -> precondition, char *, state))
                        printf("%s ", state);
	  
                    printf(GRAY "}");
                }
	
                printf("\n        wires { ");
	
                for (iterate(trigger -> wires_low, Charge *, charge))
                    if (!charge -> delay) printf(YELLOW "-" RESET "%d ", charge -> gpio);
                    else                  printf(YELLOW "-" RESET "%d(" YELLOW "%dms" RESET ") ", charge -> gpio, charge -> delay);

                for (iterate(trigger -> wires_high, Charge *, charge))
                    if (!charge -> delay) printf(YELLOW "+" RESET "%d ", charge -> gpio);
                    else                  printf(YELLOW "+" RESET "%d(" YELLOW "%dms" RESET ") ", charge -> gpio, charge -> delay);
	
                if (!trigger -> singular) printf(GRAY "}" YELLOW " *\n" GRAY);
                else                      printf(GRAY "}\n");
	
                printf("        enter { " RESET);
                for (iterate(trigger -> enter_set, Transition *, trans))
                    if (!trans -> delay) printf("%s ", trans -> state);
                    else                 printf("%s" RESET "(" YELLOW "%dms" RESET ") ", trans -> state, trans -> delay);
                printf(GRAY "}\n        leave { " RESET);
                for (iterate(trigger -> leave_set, Transition *, trans))
                    if (!trans -> delay) printf("%s ", trans -> state);
                    else                 printf("%s" RESET "(" YELLOW "%dms" RESET")", trans -> state, trans -> delay);
                printf(GRAY "}\n" RESET);
            }
        }
        printf("\n");
    }
}
