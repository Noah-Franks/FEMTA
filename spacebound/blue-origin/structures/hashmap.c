
#include "../include/program.h"

Hashmap * hashmap_create(hash_function hash, key_comparator key_diff, element_freer freer, int expected_size) {
  // Creates a hashmap with starting size as a function of expectations
  
  Hashmap * map = malloc(sizeof(Hashmap));
  
  map -> elements = 0;
  map -> size = expected_size * 2;
  
  map -> hash = hash;
  map -> key_diff = key_diff;
  map -> free = freer;
  
  map -> table = malloc(map -> size * sizeof(List **));
  map -> all   = list_create();
  map -> keys  = list_create();
  
  // Create table of linked lists
  for (int i = 0; i < map -> size; i++)
    map -> table[i] = NULL;
  
  return map;
}

void key_free(void * element) {
  free(((HashmapElement *) element) -> key);
  free(element);
}

int hash_string(void * string, int upper_bound) {
  // Hashes a string into a range using java's algorithm
  // h(x) has been chosen to minimize collisions
  // Note, hx will overflow but this is no problem
  
  int hx = 0;
  for (int i = 0; i < strlen((char *) string); i++) {
    hx = hx * 31 + ((char *) string)[i];
  }
  
  return (int) ((u32) abs(hx) % (u32) upper_bound);    // abs can be negative
}


int hash_address(void * address, int upper_bound) {
  // Hashes an address into a range
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

void * hashmap_get(Hashmap * this, void * key) {
  // gets element from hashmap
  // returns NULL when no element in found
  
  int hx = (this -> hash)(key, this -> size);
  
  List * list = this -> table[hx];
  
  if (!this -> table[hx]        ) return NULL;
  if (!this -> table[hx] -> size) return NULL;
  
  for (iterate(list, HashmapElement *, element)) {
    
    void * element_key = element -> key;
    
    if (this -> key_diff(key, element_key)) continue;
    
    return element -> value;
  }
  
  return NULL;
}

void hashmap_add(Hashmap * this, void * key, void * value) {
  // Adds an element to the hashmap
  
  int hx = this -> hash(key, this -> size);
  
  HashmapElement * element = malloc(sizeof(HashmapElement));
  element -> key = key;
  element -> value = value;
  
  if (!this -> table[hx])
    this -> table[hx] = list_that_frees(this -> free);
  
  list_insert(this -> table[hx], element);
  list_insert(this -> all, value);
  list_insert(this -> keys, key);
  
  this -> elements++;
}

bool hashmap_exists(Hashmap * this, void * key) {
  
  int hx = (this -> hash)(key, this -> size);
  
  List * list = this -> table[hx];
  
  if (!this -> table[hx]        ) return false;
  if (!this -> table[hx] -> size) return false;
  
  for (iterate(list, HashmapElement *, element)) {
    
    void * element_key = element -> key;
    
    if (this -> key_diff(key, element_key)) continue;
    
    return true;
  }
  
  return false;
}

void hashmap_remove(Hashmap * this, void * key) {
  // Removes element, making sure it exists
  // Note, this does not free the old value. If needed, do that yourself
  // Also! The all list is currently not updated!
  
  int hx = this -> hash(key, this -> size);

  List * list = this -> table[hx];

  for (iterate(list, HashmapElement *, element)) {

    void * element_key = element -> key;
    
    if (this -> key_diff(key, element_key)) continue;
    
    element_node = (void *) ((ListNode *) element_node) -> prev;    // pull-back
    
    list_remove(list, ((ListNode *) element_node) -> next);      // ---------
    //list_remove(this -> all, 
    
    this -> elements--;
    return;
  }
  
  // No match was found in the matching table list
  exit_printing(ERROR_PROGRAMMER, "Tried to remove element that does not exist");
}

void hashmap_update(Hashmap * this, void * key, void * value) {
  // Updates an element in the hashmap
  // Note this does not free the old value,
  // if freeing is necessary, do so by freeing via free(get());
  // However, I intend to use this to store indices, which are ints.
  
  int hx = this -> hash(key, this -> size);
  
  List * list = this -> table[hx];
  
  if (!list)
    exit_printing(ERROR_PROGRAMMER, "Tried to update an element that does not exist");
  
  for (iterate(list, HashmapElement *, element)) {
    
    void * element_key = element -> key;
    
    if (this -> key_diff(key, element_key)) continue;
    
    element -> value = value;
    return;
  }
  
  // No match was found in the matching table list
  exit_printing(ERROR_PROGRAMMER, "Tried to update an element that does not exist");
}

void hashmap_destroy(Hashmap * this) {

  for (int i = 0; i < this -> size; i++) {
    
    if (this -> table[i]) {
      list_destroy(this -> table[i]);
      this -> table[i] = NULL;
    }
  }
  
  list_destroy(this -> all);
  list_destroy(this -> keys);
  
  free(this -> table);
  free(this);
}


void print_hashmap_long(HashmapElement * element) {
  printf("  %s %ld\n", (char *) element -> key, (long) element -> value);
}

void hashmap_print(Hashmap * this, element_printer print) {
  // prints each element in the hash map

  printf("Hashmap of size %d\n",   this -> size);
  printf("Contains %d elements\n", this -> elements);
  
  for (int i = 0; i < this -> size; i++) {
    
    if (!this -> table[i]) continue;
    
    printf("%d\n", i);
    
    List * list = this -> table[i];
    
    for (iterate(list, HashmapElement *, element))
      print(element);
  }
}
