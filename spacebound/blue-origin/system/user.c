
#include "../include/program.h"

local void output_str(void * nil, char * raw_text) {
  /* print a message to the console */
  
  if (strlen(raw_text) < 2) printf("user message: \n");
  else                      printf("user message: %s\n", raw_text + 1);
}

local void flip_bool(void * pointer, char * raw_text) {
  /* Flips a boolean value */
  
  *(bool *) pointer = ! *(bool *) pointer;
}

void present_interface() {
  
  prctl(PR_SET_NAME, "Console", NULL, NULL, NULL);
  
  bool reading_user_input = true;
  
  Hashmap * actions = hashmap_create(hash_address, compare_addresses, NULL, NULL, 8);
  Hashmap * args    = hashmap_create(hash_address, compare_addresses, NULL, NULL, 8);
  
  hashmap_add(actions, (void *) 'q',    flip_bool);  hashmap_add(args, (void *) 'q',     &reading_user_input);
  hashmap_add(actions, (void *) 'e',    flip_bool);  hashmap_add(args, (void *) 'e', &console_error_messages);
  hashmap_add(actions, (void *) 'm',   output_str);  hashmap_add(args, (void *) 'm',                    NULL);
  hashmap_add(actions, (void *) '+',  pin_set_hot);  hashmap_add(args, (void *) '+',                    NULL);
  hashmap_add(actions, (void *) '-', pin_set_cold);  hashmap_add(args, (void *) '-',                    NULL);
  hashmap_add(actions, (void *) 'p',   flip_print);  hashmap_add(args, (void *) 'p',                    NULL);
  
  char input[32];
  
  while (reading_user_input) {
    
    fgets(input, 32, stdin);
    
    user_action action = hashmap_get(actions, (void *) (int) input[0]);
    void *      arg    = hashmap_get(args   , (void *) (int) input[0]);
    
    if (action)
      action(arg, input);
  }
  
  hashmap_delete(actions);
  hashmap_delete(args);
}
