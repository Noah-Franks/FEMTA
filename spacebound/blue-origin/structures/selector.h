#pragma once

#include "../include/headers.h"

typedef struct Command {
  
  char            key;
  char *          text;
  selector_action action;
  void *          argument;
  
} Command;

typedef struct Selector {

  char * title;
  
  List * entries;
  
  struct Selector * parent;

} Selector;

extern bool reading_user_input;
