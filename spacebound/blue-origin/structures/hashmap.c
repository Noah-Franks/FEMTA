
#include "../include/program.h"

Hashmap * hashmap_create(hash_function hash, key_comparator key_diff, freer key_free, freer value_free, int size) {
  
  Hashmap * map = calloc(1, sizeof(*map));
  
  map -> size       = size;
  map -> hash       = hash;
  map -> key_diff   = key_diff;
  map -> key_free   = key_free;
  map -> value_free = value_free;
  
  map -> table = calloc(map -> size, sizeof(*map -> table));
  map -> all   = list_create();
  map -> keys  = list_create();
  
  return map;
}

void * hashmap_delete(Hashmap * map) {
  // completely deletes a hashmap, deleting all content in the process
  
  for (int i = 0; i < map -> size; i++) {
    
    if (map -> table[i]) {
      
      for (iterate(map -> table[i], HashmapElement *, element)) {
        if (map ->   key_free)(map ->   key_free)(element -> key  );
        if (map -> value_free)(map -> value_free)(element -> value);
      }
      
      map -> table[i] = list_delete(map -> table[i]);
    }
  }
  
  map -> all  = list_delete(map -> all );    // these don't need a value_free
  map -> keys = list_delete(map -> keys);    // -----------------------------
  
  blank(map -> table);
  free(map);
  return NULL;
}

void hashmap_add(Hashmap * map, void * key, void * value) {
  // adds an element to the hashmap. (assumes value doesn't already exist).
  
  int hx = (map -> hash)(key, map -> size);
  
  HashmapElement * element = malloc(sizeof(*element));
  element -> key   = key;
  element -> value = value;
  
  if (!map -> table[hx])
    map -> table[hx] = list_that_frees(free);        // will free the container element
  
  list_insert(map -> table[hx], element);            // add to table row
  list_insert(map -> all, value);                    // add to list of entries
  list_insert(map -> keys, key);                     // add to list of keys
  
  map -> elements++;
}

local HashmapElement * hashmap_find(Hashmap * map, void * key) {
  // finds the element containing the key value pair.
  
  int hx = (map -> hash)(key, map -> size);
  
  List * list = map -> table[hx];
  
  if (!map -> table[hx]            ) return NULL;    // nothing hashes to this value
  if (!map -> table[hx] -> elements) return NULL;    // ----------------------------
  
  for (iterate(list, HashmapElement *, element)) {
    
    void * element_key = element -> key;
    
    when (!map -> key_diff(key, element_key));       // see if keys match
    
    return element;                                  // match found
  }
  
  return NULL;                                       // no match found
}

void * hashmap_get(Hashmap * map, void * key) {
  // gets element from hashmap, returning NULL if none is found.
  
  if (!key) return NULL;                             // NULL maps to nothing
  
  HashmapElement * element = hashmap_find(map, key);
  
  if (element) return element -> value;
  else         return NULL;
}

bool hashmap_exists(Hashmap * map, void * key) {
  return !!hashmap_find(map, key);    // sees if an element with this key exists in the hashmap
}

void hashmap_update(Hashmap * map, void * key, void * value) {
  // updates an element in the hashmap, freeing the old one if needed.
  
  HashmapElement * element = hashmap_find(map, key);
  
  if (!element)
    exit_printing(ERROR_PROGRAMMER, "Tried to update an element that does not exist");
  
  if (map -> value_free)                      // delete old value
    (map -> value_free)(element -> value);    // ----------------
  
  element -> value = value;                   // assign new value
}

void vhashmap_delete(Hashmap * map) {         // void version of hashmap delete
  hashmap_delete(map);
}

/* Common functions used when creating hashmaps */

int hash_string(void * string, u32 upper_bound) {      // hashes a string using java's algorithm.
  
  u32 hx = 0;
  for (int i = 0; i < strlen((char *) string); i++)
    hx = hx * 31 + ((char *) string)[i];
  
  return (int) (hx % upper_bound);
}

int hash_address(void * address, u32 upper_bound) {    // hashes an address using logic found on StackOverflow
  
  u32 hx = (u32) address;
  
  hx = ((hx >> 16) ^ hx) * 0x119de1f3;                 // poster empirically found the constant used
  hx = ((hx >> 16) ^ hx) * 0x119de1f3;                 // ------------------------------------------
  hx = ((hx >> 16) ^ hx);                              // ------------------------------------------
  
  return hx % upper_bound;
}

int compare_strings(void * first, void * other) {      // wraps strcmp
  return strcmp(first, other);
}

int compare_addresses(void * first, void * other) {    // wraps numerical comparison
  return (int) (first != other);
}
