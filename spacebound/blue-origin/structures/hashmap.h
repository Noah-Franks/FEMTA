#pragma once

#include "../include/headers.h"

typedef struct HashmapElement {
  
  void * key;                 // The key used to access this value
  void * value;               // The content this element holds
  
} HashmapElement;


typedef struct Hashmap {
  
  int elements;               // number of elements stored in the map
  int size;                   // number of rows in the table
  
  List ** table;              // output values
  List *  all;                // all values in table    (do not use with remove!)
  List *  keys;               // all keys in table      -------------------------
  
  freer key_free;             // means of freeing hashmap element key
  freer value_free;           // means of freeing hashmap element value
  
  hash_function  hash;        // how hash table indecies are calculated
  key_comparator key_diff;    // how to tell if keys match
  
} Hashmap;
