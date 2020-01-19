
#include "../include/program.h"

Hashmap * hashmap_create(hash_function hash, key_comparator key_diff, freer key_free, freer value_free, int size) {
  // creates a hashmap with starting size as a function of expectations
  
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

void hashmap_delete(Hashmap * map) {
  
  for (int i = 0; i < map -> size; i++) {
    
    if (map -> table[i]) {
      
      for (iterate(map -> table[i], HashmapElement *, element)) {
        if (map ->   key_free)(map ->   key_free)(element -> key  );
        if (map -> value_free)(map -> value_free)(element -> value);
      }
      
      list_delete(map -> table[i]);
      map -> table[i] = NULL;
    }
  }
  
  list_delete(map -> all);
  list_delete(map -> keys);
  
  blank(map -> table);
  free(map);
}

void * hashmap_get(Hashmap * map, void * key) {
  // gets element from hashmap.
  // returns NULL when no element in found.
  
  if (!key) 
    return NULL;
  
  int hx = (map -> hash)(key, map -> size);
  
  List * list = map -> table[hx];
  
  if (!map -> table[hx]        ) return NULL;
  if (!map -> table[hx] -> size) return NULL;
  
  for (iterate(list, HashmapElement *, element)) {
    
    void * element_key = element -> key;
    
    when (!map -> key_diff(key, element_key));
    
    return element -> value;
  }
  
  return NULL;
}

void hashmap_add(Hashmap * map, void * key, void * value) {
  // adds an element to the hashmap
  
  int hx = map -> hash(key, map -> size);
  
  HashmapElement * element = malloc(sizeof(*element));
  element -> key = key;
  element -> value = value;
  
  if (!map -> table[hx])
    map -> table[hx] = list_that_frees(free);
  
  list_insert(map -> table[hx], element);
  list_insert(map -> all, value);
  list_insert(map -> keys, key);
  
  map -> elements++;
}

bool hashmap_exists(Hashmap * map, void * key) {
  
  int hx = (map -> hash)(key, map -> size);
  
  List * list = map -> table[hx];
  
  if (!map -> table[hx]        ) return false;
  if (!map -> table[hx] -> size) return false;
  
  for (iterate(list, HashmapElement *, element)) {
    
    void * element_key = element -> key;
    
    when (!map -> key_diff(key, element_key));
    
    return true;
  }
  
  return false;
}

void hashmap_remove(Hashmap * map, void * key) {
  // removes element, making sure it exists
  // note, this must never be done when wanting to use the all or key lists
  
  int hx = map -> hash(key, map -> size);
  
  List * list = map -> table[hx];
  
  for (iterate(list, HashmapElement *, element)) {
    
    void * element_key = element -> key;
    
    when (!map -> key_diff(key, element_key));
    
    element_node = (void *) ((ListNode *) element_node) -> prev;    // pull-back
    list_remove(list, ((ListNode *) element_node) -> next);         // ---------
    
    map -> elements--;
    return;
  }
  
  // No match was found in the matching table list
  exit_printing(ERROR_PROGRAMMER, "Tried to remove element that does not exist");
}

void hashmap_update(Hashmap * map, void * key, void * value) {
  // updates an element in the hashmap, freeing the old one if needed.
  
  int hx = map -> hash(key, map -> size);
  
  List * list = map -> table[hx];
  
  if (!list)
    exit_printing(ERROR_PROGRAMMER, "Tried to update an element that does not exist");
  
  for (iterate(list, HashmapElement *, element)) {
    
    void * element_key = element -> key;
    
    when (!map -> key_diff(key, element_key));
    
    if (map -> value_free)
      (map -> value_free)(element -> value);
    
    element -> value = value;
    return;
  }
  
  // No match was found in the matching table list
  exit_printing(ERROR_PROGRAMMER, "Tried to update an element that does not exist");
}

void print_hashmap_long(HashmapElement * element) {
  printf("  %s %ld\n", (char *) element -> key, (long) element -> value);
}

void hashmap_print(Hashmap * map, element_printer print) {
  // prints each element in the hash map
  
  printf("Hashmap of size %d\n",   map -> size);
  printf("Contains %d elements\n", map -> elements);
  
  for (int i = 0; i < map -> size; i++) {
    
    if (!map -> table[i]) continue;
    
    printf("%d\n", i);
    
    List * list = map -> table[i];
    
    for (iterate(list, HashmapElement *, element))
      print(element);
  }
}

int hash_string(void * string, int upper_bound) {
  // hashes a string into a range using java's algorithm
  // h(x) has been chosen to minimize collisions.
  // note, hx will overflow but this is no problem
  
  int hx = 0;
  for (int i = 0; i < strlen((char *) string); i++)
    hx = hx * 31 + ((char *) string)[i];
  
  return (int) ((u32) abs(hx) % (u32) upper_bound);    // abs can be negative
}

int hash_address(void * address, int upper_bound) {
  // hashes an address into a range.
  // I found this specific 32-bit hashing function on Stack Overflow
  
  int hx = (int) ((long) address);
  
  hx = ((hx >> 16) ^ hx) * 0x119de1f3;
  hx = ((hx >> 16) ^ hx) * 0x119de1f3;
  hx = ((hx >> 16) ^ hx);
  
  return hx % upper_bound;
}

int compare_strings(void * first, void * other) {
  return strcmp(first, other);
}

int compare_addresses(void * first, void * other) {
  return (int) (first != other);
}
