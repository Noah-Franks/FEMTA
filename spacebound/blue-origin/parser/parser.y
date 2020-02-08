
%{
#include "../include/program.h"

FILE * yyin;

int  yylex();
void yyerror(char * message, ...) opt(noreturn);

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

%token IF SET ENTER LEAVE AFTER STATE PIN POS NEG WHERE IS DEFINE SENSOR

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

Config   : Defs Sensors                                    {                                                      }
         | Defs                                            { printf("\nExperiment does not use sensors\n");       }
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

Sensor   : SENSOR ID NUMERIC             '{'       '}'     { build_sensor($2, $3, NULL,  NULL);                   }
         | SENSOR ID NUMERIC             '{' Specs '}'     { build_sensor($2, $3, NULL,    $5);                   }
         | SENSOR ID NUMERIC '/' NUMERIC '{'       '}'     { build_sensor($2, $3,   $5,  NULL);                   }
         | SENSOR ID NUMERIC '/' NUMERIC '{' Specs '}'     { build_sensor($2, $3,   $5,    $7);                   }
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
         | WHERE ID IS ID                                  { $$ = make_tag(strdup("alias"), list_from(2, $2, $4), NULL);  }
         ;

Args     : Args ',' NUMERIC                                { list_insert($1, $3); $$ = $1;                        }
         | NUMERIC                                         { $$ = list_from(1, $1);                               }
         ;

Options  : ID                                              { $$ = list_from(1, $1);                               }
         | Options ',' ID                                  { list_insert($1, $3); $$ = $1;                        }
         ;

%%

local int n_triggers = 0;

local void check_unit(char * unit_name) {
    if (!unit_is_supported(unit_name))
        yyerror("Unknown unit " CYAN "%s", unit_name);
}

local void check_target(Sensor * proto, char * target) {
    if (!proto -> targets || !hashmap_exists(proto -> targets, target))
        yyerror("Target " CYAN "%s " RED "for %s is not known", target, proto -> code_name);
}

local void represent_as_decimal(Numeric * numeric) {
    
    if (numeric -> is_decimal) return;
    
    numeric -> decimal    = (float) numeric -> integer;
    numeric -> is_decimal = true;
    
    if (!strcmp(numeric -> units, "i"))    // change generic unit if needed
      numeric -> units[0] = 'f';           // -----------------------------
}

local void represent_as_integer(Numeric * numeric) {
    
    if (!numeric -> is_decimal) return;
    
    numeric -> integer    = (int) numeric -> decimal;
    numeric -> is_decimal = false;
    
    if (!strcmp(numeric -> units, "f"))      // change generic unit if needed
        numeric -> units[0] = 'i';           // -----------------------------
}

local void specification_delete(void * vspecification) {
    
    Specification * specification = vspecification;
    
    blank(specification -> id);
    free(specification);
}

void check_and_add_state(char * name, bool entered) {
  
  if (state_exists(name))
      yyerror("State " CYAN "%s " RED "already defined", name);
  
  add_state(name, entered);
}

EffectNode * make_charge(Numeric * wire, bool hot) {
    
    if (!unit_is_of_type(wire, "Integer"))
        yyerror("Wire %s is not an integer", wire -> units);
    
    if (!between(0, wire -> integer, 27))
        yyerror("Broadcom " CYAN "%d " RED "does not exist", wire -> integer);
    
    Charge * charge = calloc(1, sizeof(*charge));
    
    charge -> gpio = wire -> integer;    // delays are added later
    
    blank(wire);
    
    EffectNode * effect = calloc(1, sizeof(*effect));
    
    effect -> charge = charge;
    effect -> hot    = hot;
    
    effect -> is_charge = true;
    return effect;
}

EffectNode * make_transition(char * state_name, bool entering) {
    
    if (!state_exists(state_name))
        yyerror("Unknown state name: " CYAN "%s", state_name);
    
    EffectNode * effect  = calloc(1, sizeof(*effect));
    
    effect -> is_charge  = false;
    effect -> transition = transition_create(state_name, 0);    // delays are added later
    effect -> entering   = entering;
    
    return effect;
}

EffectNode * add_delay(EffectNode * effect, Numeric * delay) {
    
    represent_as_decimal(delay);
    
    if (delay -> decimal <= 0)
        yyerror("Delay %f not allowed: value is non-positive", delay -> decimal);
    
    if (!unit_is_supported(delay -> units) || !unit_is_of_type(delay, "Time"))
        yyerror("%s is not a unit of time", delay -> units);
    
    effect -> delay_ms = (get_universal_conversion(delay -> units, "ms"))(delay -> decimal);
    
    if (effect -> is_charge) effect -> charge     -> delay = (int) effect -> delay_ms;
    else                     effect -> transition -> delay = (int) effect -> delay_ms;
    
    if (!effect -> is_charge)
      state_inform_delays(effect -> transition -> state);
    
    blank(delay);
    return effect;
}

Trigger * make_trigger(List * effects) {
    // creates a trigger, which becomes a specification later
    
    Trigger * trigger = calloc(1, sizeof(*trigger));
    
    trigger -> fired    = false;
    trigger -> singular = false;    // defaults
    trigger -> reverses = false;    // --------
    
    trigger -> wires_low  = list_that_frees(free);
    trigger -> wires_high = list_that_frees(free);
    trigger -> enter_set  = list_that_frees(transition_delete);
    trigger -> leave_set  = list_that_frees(transition_delete);
    
    for (iterate(effects, EffectNode *, effect))
        if (effect -> is_charge)
            if (effect -> hot     ) list_insert(trigger -> wires_high, effect -> charge    );
            else                    list_insert(trigger -> wires_low , effect -> charge    );
        else
            if (effect -> entering) list_insert(trigger -> enter_set , effect -> transition);
            else                    list_insert(trigger -> leave_set , effect -> transition);
    
    effects -> value_free = free;
    list_delete(effects);
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
    // Wraps and modifies the trigger into a specification so that sensor construction
    // have a linked list consisting of nodes with the same content
    
    if (state_names) {
        for (iterate(state_names, char *, state_name))
            if (!state_exists(state_name))
                yyerror("Unknown state name " CYAN "%s", state_name);
        
        state_names -> value_free = free;
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
            if      (!strcmp(option, "single" )) trigger -> singular =  true;
            else if (!strcmp(option, "forever")) trigger -> singular = false;
            else if (!strcmp(option, "reverse")) trigger -> reverses =  true;
            else    yyerror("Unknown option " CYAN "%s", option);
        }
        
        options -> value_free = free;
        list_delete(options);
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
        
        if (!options || !options -> elements)
            yyerror("Please specify at least one channel to print");
        
        char * color = list_get(options, 0);
        
        if (!get_color_by_name(color))
            list_insert_first(options, strdup("white"));
        
        if (options -> elements == 1) 
            yyerror("Please specify at least one channel to print");
        
        if (args) {
            for (iterate(args, Numeric *, numeric)) {
                
                represent_as_integer(numeric);
                
                if (numeric -> integer < 0)
                    yyerror("Negative number of decimal places " CYAN "%d " RED "specified", numeric -> integer);
            }
        }
    }
    
    else if (!strcmp(id, "smooth")) {
        
        if (!options || options -> elements != 1) yyerror("Smoothing requires exactly 1 argument (the target)");
        
        if (args) {
            Numeric * numeric = list_get(args, 0);
            float value = numeric -> decimal;
            
            if (strcmp(numeric -> units, "f") || value < 0.0f || value > 1.0f)
                yyerror("Autoregressive smoothing constant " CYAN "%f " RED "not a float in [0.0, 1.0]", value);
        }
    }
    
    else if (!strcmp(id, "calibrate")) {
        
        if (!args   )                yyerror("Calibration curves require at least one constant");
        if (!options)                yyerror("Calibration requires a target");
        if (options -> elements < 4) yyerror("Calibration requires a unit 'from' followed by a unit 'to'");
        
        char * curve = (char *) list_get(options, 1);
        
        if      (!strcmp(curve, "poly"));
        else if (!strcmp(curve, "hart")) {
            if (args -> elements != 3)
                yyerror("The Steinhart and Hart Equation requires exactly 3 constants");
        }
        else {
            yyerror("Unknown calibration curve " CYAN "%s", curve);
        }
        
        char * unit_from = list_get(options, 2);
        char * unit_to   = list_get(options, 3);
        
        check_unit(unit_from);
        check_unit(unit_to);
    }
    
    else if (!strcmp(id, "conversions")) {
        
        if (args                               ) yyerror("A conversion series should never have arguments"         );
        if (!options || options -> elements < 3) yyerror("A conversion series must include a target, then 2+ units");
        if (strcmp(list_get(options, 1), "raw")) yyerror("A conversion series always starts with 'raw'"            );
        
        for (iterate(options, char *, unit_name)) {
            
            if ((int) unit_name_index < 1) continue;    // first is target, so skip (see list.h)
            
            check_unit(unit_name);
        }
    }
    
    else if (!strcmp(id, "alias")) {
        if (args                                ) yyerror("Aliasing takes no arguments"                         );
        if (!options || options -> elements != 2) yyerror("Aliasing takes 2 options, the channel and alias name");
    }
    
    else {
        yyerror("Invalid tag id " CYAN "%s", id);
    }
    
    return tag;
}

void build_sensor(char * id, Numeric * frequency, Numeric * denominator, List * specifications) {
    
    Sensor * proto = hashmap_get(all_sensors, id);
    
    if (!proto)
        yyerror(CYAN "%s " RED "is not a sensor", id);
    
    blank(id);
    
    if (strcmp(frequency   -> units, "Hz") && strcmp(frequency   -> units, "i"))
        yyerror("Sensor frequency using %s is neither an integer nor a rational number of Hz", frequency -> units);
    
    if (denominator && strcmp(denominator -> units, "Hz") && strcmp(denominator -> units, "i"))
        yyerror("Sensor frequency using %s is neither an integer nor a rational number of Hz", denominator -> units);
    
    proto -> hertz = frequency -> integer;    // base Hz for scheduling
    
    if (denominator) proto -> hertz_denominator = denominator -> integer;
    else             proto -> hertz_denominator = 0;
    
    blank(frequency);
    blank(denominator);
    
    proto -> requested = true;
    
    for (int stream = 0; stream < proto -> data_streams; stream++)
        proto -> outputs[stream].triggers = list_that_frees(trigger_delete);
    
    if (!specifications) return;
    
    
    /* collect all the aliases */
    
    Hashmap * aliases = proto -> aliases;
    
    for (iterate(specifications, Specification *, specification)) {
        
        when (!strcmp(specification -> id, "alias"));
        
        List * options = specification -> options;
        
        char * target = list_get(options, 0);
        char * alias  = list_get(options, 1);
        
        check_target(proto, target);
        
        if (hashmap_exists(proto -> targets, alias))
            yyerror("Tried to alias " CYAN "%s" RED "'s " CYAN "%s " RED "channel as itself", proto -> code_name, alias);
        
        if (hashmap_exists(proto -> targets, alias))
            yyerror("Found more than one alias for " CYAN "%s" RED "'s " CYAN "%s", proto -> code_name, target);
        
        // use the alias for the log file header
        proto -> outputs[(int) hashmap_get(proto -> targets, target)].nice_name = alias;
        
        hashmap_add(aliases, alias, target);
        list_delete(specification -> options);
        specification -> options = NULL;          // protection from freer assignment later
    }
    
    
    /* replace aliases */
    
    for (iterate(specifications, Specification *, specification)) {
        
        List * options = specification -> options;
        
        if (!strcmp(specification -> id, "trigger")) {
            
            Trigger * trigger = list_get(options, 0);    // we encapsulated the trigger within the options list
            
            when (hashmap_exists(aliases, trigger -> id));
            
            char * unaliased = strdup(hashmap_get(aliases, trigger -> id));
            
            free(trigger -> id);
            
            trigger -> id = unaliased;
        }
        
        else if (!strcmp(specification -> id, "print")) {
          
          for (iterate(options, char *, target)) {
            
            when ((int) target_index > 0         );    // first is color, so skip (see list.h)
            when (hashmap_exists(aliases, target));
            
            char * unaliased = strdup(hashmap_get(aliases, target));
            
            blank(target);
            
            ((ListNode *) target_node) -> value = unaliased;
          }
        }
        
        else if (!strcmp(specification -> id, "smooth")    ||
                 !strcmp(specification -> id, "calibrate") ||
                 !strcmp(specification -> id, "conversions")) {
          
          char * target = list_get(options, 0);
          
          when (hashmap_exists(aliases, target));
          
          char * unaliased = strdup(hashmap_get(aliases, target));
          
          blank(target);
          
          list_get(options, 0) = unaliased;    // works due to compiler (list.h)
        }
        
    }
    
    
    
    List * all_calibrations = list_create();
    
    for (iterate(specifications, Specification *, specification)) {
        
        List * options = specification -> options;
        List * args    = specification -> args;
        
        if (options) options -> value_free = free;
        if (args   ) args    -> value_free = free;
        
        if (!strcmp(specification -> id, "trigger")) {
            
            // can't yet do conversion, reversals, etc,
            // so we do the rest in a later for loop
            
            Trigger * trigger = list_get(options, 0);    // we encapsulated the trigger within the options list
            
            check_target(proto, trigger -> id);
            
            int stream = (int) hashmap_get(proto -> targets, trigger -> id);
            
            list_insert(proto -> outputs[stream].triggers, trigger);
            options -> value_free = NULL;
            list_delete(options);
        }
        
        else if (!strcmp(specification -> id, "print")) {
            
            char * color      = list_get(options, 0);
            char * print_code = get_color_by_name(color);
            
            for (iterate(options, char *, target)) {
                
                if ((int) target_index < 1) continue;    // first is target, so skip (see list.h)
                
                check_target(proto, target);
                
                int stream = (int) hashmap_get(proto -> targets, target);
                
                proto -> outputs[stream].print        = true;
                proto -> outputs[stream].print_code   = print_code;
                proto -> outputs[stream].print_places = 2;
                
                if (args && (int) target_index - 1 < args -> elements) {
                    
                    Numeric * places = list_retrieve(args, (int) target_index - 1);
                    
                    proto -> outputs[stream].print_places = places -> integer;
                }
            }
            
            list_delete(options);
            
            if (args)
                list_delete(args);
            
            proto -> print = true;
        }
        
        else if (!strcmp(specification -> id, "smooth")) {
            
            Numeric * numeric = list_get(args,    0);
            char    * target  = list_get(options, 0);
            
            float regressive = numeric -> decimal;
            
            check_target(proto, target);
            
            int stream = (int) hashmap_get(proto -> targets, target);
            
            if (proto -> outputs[stream].regressive != 1.0f)    // not the same as just 'regressive'
                yyerror("Found second smoothing term " CYAN "%f " RED " for " CYAN "%s", regressive, proto -> code_name);
            
            proto -> outputs[stream].regressive = regressive;
            
            list_delete(options);
            list_delete(args);
        }
        
        else if (!strcmp(specification -> id, "calibrate")) {
            
            /* as with the triggers, we can't do everything here (like forming each output series) *
             * so we'll place each calibration (which are themselves lists) into an UNORDERED list *
             * for each target using the calibrations hashmap. Later, we'll actually convert these *
             * unordered lists into proper serieses.                                               */
            
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
            calibration -> constants = args;         // the args list
            calibration -> unit_from = unit_from;
            calibration -> unit_to   = unit_to;
            calibration -> target    = target;
            
            list_insert(all_calibrations, calibration);
            
            // check bellow
            options -> value_free = NULL;
            list_delete(options);
        }
        
        else if (!strcmp(specification -> id, "conversions")) {
            
            char * target = list_get(options, 0);
            
            check_target(proto, target);
            
            int stream = (int) hashmap_get(proto -> targets, target);
            
            if (proto -> outputs[stream].series)
                yyerror("Found second conversion series for %s's " CYAN "%s", proto -> code_name, target);
            
            // temporarily store the options list. this gets replaced
            // with the actual series when iterating over the calibrations
            list_remove(options, options -> head);        // remove the target so only unit names exist
            proto -> outputs[stream].series = options;
        }
    }
    
    
    for (int stream = 0; stream < proto -> data_streams; stream++) {
        
        char * output_name = proto -> outputs[stream].name;
        
        List * unit_names = proto -> outputs[stream].series;    // get the temporarily stored unit names (raw -> measure)
        
        // make sure in the event no series was specified that there's no calibrations
        if (!unit_names) {
            for (iterate(all_calibrations, Calibration *, calibration)) {
                if (stream == (int) hashmap_get(proto -> targets, calibration -> target))
                    yyerror("Found calibration without conversions series for " CYAN "%s", calibration -> target);
            }
            continue;    // no unit names means no conversion series
        }
        
        List * series = list_that_frees(series_element_delete);
        
        char * last_name = list_get(unit_names, 0);
        
        for (iterate(unit_names, char *, name)) {
            
            if (!strcmp(last_name, name)) continue;    // nothing need be done when same
            
            // see if sensor has specified a calibration for this unit
            
            SeriesElement * element = NULL;
            
            for (iterate(all_calibrations, Calibration *, calibration)) {
                if (strcmp(output_name, calibration -> target   )) continue;
                if (strcmp(  last_name, calibration -> unit_from)) continue;
                if (strcmp(       name, calibration -> unit_to  )) continue;
                
                calibration -> used = true;    // mark this to delay deletion
                
                element = series_element_from_calibration(calibration);
            }
            
            if (!element)   // does error checking
                element = series_element_from_conversion(get_universal_conversion(last_name, name));
            
            list_insert(series, element);
            last_name = name;
        }
        
        proto -> outputs[stream].unit   = strdup(last_name);
        proto -> outputs[stream].series = series;
        list_delete(unit_names);
    }
    
    for (iterate(all_calibrations, Calibration *, calibration))
        if (!calibration -> used)
            calibration_delete(calibration);
    
    list_delete(all_calibrations);
    all_calibrations = NULL;
    
    
    for (int stream = 0; stream < proto -> data_streams; stream++) {
        
        if (!proto -> outputs[stream].unit)
            proto -> outputs[stream].unit = strdup("raw");
        
        char * final_unit = proto -> outputs[stream].unit;
        List * triggers   = proto -> outputs[stream].triggers;
        
        // perform conversions
        for (iterate(triggers, Trigger *, trigger)) {
            
            Numeric * before = trigger -> threshold_as_specified;
            
            trigger -> threshold = (get_universal_conversion(before -> units, final_unit))(before -> decimal);
        }
        
        // duplicate reversing triggers
        for (iterate(triggers, Trigger *, trigger)) {
            
            when (trigger -> reverses);
            
            Trigger * opposite = calloc(1, sizeof(*opposite));
            
            opposite -> id       =  trigger -> id;
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
        
        n_triggers += triggers -> elements;
    }
    
    specifications -> value_free = specification_delete;    // Note: up to us to delete lists earlier
    list_delete(specifications);                            // ---------------------------------------
}

void print_config() {
    
    if (!n_triggers) {
        printf("\n\nExperiment does not involve triggers\n\n");
        return;
    }
    
    print_all_states();
    
    for (iterate(all_sensors -> all, Sensor *, proto)) {
        
        if (!proto -> requested) continue;
        
        int total_triggers = 0;
        for (int stream = 0; stream < proto -> data_streams; stream++)
            total_triggers += proto -> outputs[stream].triggers -> elements;
        
        if (!total_triggers) continue;
        
        printf(GREEN "%s\n" RESET, proto -> code_name);
        
        for (int stream = 0; stream < proto -> data_streams; stream++) {
            
            char * final_unit = proto -> outputs[stream].unit;
            
            for (iterate(proto -> outputs[stream].triggers, Trigger *, trigger)) {
                
                Numeric * before = trigger -> threshold_as_specified;
                char   direction = (trigger -> less) ? '<' : '>';
                
                printf(CYAN "    %s" GRAY " %c" MAGENTA " %.3f%s " GRAY "(= " MAGENTA "%.3f%s" GRAY ")",
                       trigger -> id, direction, before -> decimal, before -> units, trigger -> threshold, final_unit);
                
                if (trigger -> precondition -> elements) {
                    
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
