#pragma once

#include <stdbool.h>

#include "list.h"

typedef struct HashmapElement HashmapElement;

typedef int (* hash_function)(void *, int upper_bound);
typedef int (* key_comparator)(void * first, void * other);
typedef void (* element_freer)(void * element);
typedef void (* element_printer)(HashmapElement * element);

typedef struct HashmapElement {
  
  void * key;       // The key used to access this value
  void * value;     // The content this element points to
  
} HashmapElement;


typedef struct Hashmap {
  
  int elements;               // number of elements stored in the map
  int size;                   // number of rows in the table
  
  List ** table;              // output values
  List * all;                 // all values in table (bad after remove is called)
  List * keys;                // all keys in table (bad after remove is called)
  
  element_freer free;         // means of freeing hashmap element
  hash_function hash;         // how hash table indecies are calculated
  key_comparator key_diff;    // how to tell if keys match
  
} Hashmap;

Hashmap * hashmap_create(hash_function hash, key_comparator key_diff, element_freer freer, int expected_size);

void key_free(void * element);

int hash_string (void *  string, int upper_bound);    // (char *, int)
int hash_address(void * address, int upper_bound);    // (void *, int)

int compare_strings  (void * first, void * other);    // (char *, char *)
int compare_addresses(void * first, void * other);    // (void *, void *)

void print_hashmap_long(HashmapElement * element);


void * hashmap_get    (Hashmap * this, void * key);
void   hashmap_add    (Hashmap * this, void * key, void * value);
void   hashmap_update (Hashmap * this, void * key, void * value);
bool   hashmap_exists (Hashmap * this, void * key);
void   hashmap_remove (Hashmap * this, void * key);
void   hashmap_print  (Hashmap * this, element_printer print);
void   hashmap_destroy(Hashmap * this);
