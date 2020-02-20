
#include "../include/program.h"

local ListNode * list_create_node(void * value) {      // creates node storing value
  ListNode * node = calloc(1, sizeof(*node));
  node -> value = value;
  return node;
}

List * list_create() {                                 // creates an empty list
  return calloc(1, sizeof(List));
}

List * list_from(int nargs, ...) {                     // creates a list holding the arguments
  
  List * list = list_create();
  
  va_list args;
  va_start(args, nargs);
  
  for (int i = 0; i < nargs; i++)
    list_insert(list, va_arg(args, void *));
  
  va_end(args);
  return list;
}

List * list_that_frees(freer value_free) {             // creates a list that frees its elements
  List * list = list_create();
  list -> value_free = value_free;
  return list;
}

void list_insert(List * list, void * value) {          // inserts node at the tail in O(1)
  
  list -> elements++;
  
  ListNode * node = list_create_node(value);
  
  if (!list -> head) {
    list -> head = node;                               // a list with only 1 node points to itself
    node -> prev = node;                               // ----------------------------------------
    node -> next = node;                               // ----------------------------------------
    return;
  }
  
  node -> next = list -> head;
  node -> prev = list -> head -> prev;
  
  list -> head -> prev -> next = node;
  list -> head -> prev = node;
}

void list_insert_first(List * list, void * value) {    // inserts node at the head in O(1)
  list_insert(list, value);                            // insert into the list, but roll the tail
  list -> head = list -> head -> prev;                 // ---------------------------------------
}

void * list_retrieve(List * list, int index) {         // retrieves the ith element in the list in O(n)
  
  if (unlikely(index >= list -> elements))
    exit_printing(ERROR_PROGRAMMER, "list_retrieve: index out of bounds");
  
  for (iterate(list, void *, entry))
    if ((int) entry_index == index)
      return entry;
  
  exit_printing(ERROR_PROGRAMMER, "list corruption observed when retrieving element %d", index);
}

void list_remove(List * list, ListNode * node) {       // removes a node from the list in O(1), freeing if necessary
  
  node -> next -> prev = node -> prev;                 // Drop out of DLL
  node -> prev -> next = node -> next;                 // ---------------
  
  if (node == list -> head)                            // Advance head if necessary
    list -> head = node -> next;                       // -------------------------
  
  if (--list -> elements == 0) list -> head = NULL;
  
  if (list -> value_free) (list -> value_free)(node -> value);
  free(node);
}

void * list_delete(List * list) {                      // completely delete the list and its elements
  
  while (list -> elements)
    list_remove(list, list -> head);
  
  free(list);
  return NULL;
}
