
#include "../include/program.h"

ListNode * list_create_node(void * value) {
  ListNode * node = malloc(sizeof(*node));
  node -> value = value;
  node -> next = NULL;
  node -> prev = NULL;
  return node;
}

List * list_create() {
  return calloc(1, sizeof(List));
}

List * list_from(int nargs, ...) {
  // creates a list from arguments
  
  List * list = list_create();
  
  va_list args;
  va_start(args, nargs);
  
  for (int i = 0; i < nargs; i++)
    list_insert(list, va_arg(args, void *));
  
  va_end(args);
  return list;
}

List * list_that_frees(freer value_free) {
  // specifies a free-ing function that gets called on each
  // node's datum when deleting the list.
  
  List * list = list_create();
  list -> value_free = value_free;
  return list;
}

void list_insert(List * list, void * value) {
  // inserts node at the tail in constant time
  
  list -> size++;
  
  ListNode * node = list_create_node(value);
  
  if (list -> head == NULL) {
    list -> head = node;
    node -> prev = node;
    node -> next = node;
    return;
  }
  
  node -> next = list -> head;
  node -> prev = list -> head -> prev;
  
  list -> head -> prev -> next = node;
  list -> head -> prev = node;
}

void list_insert_first(List * list, void * value) {
  // Inserts node at the head in constant time
  
  list -> size++;
  
  ListNode * node = list_create_node(value);
  
  if (list -> head == NULL) {
    list -> head = node;
    node -> prev = node;
    node -> next = node;
    return;
  }
  
  node -> next = list -> head;
  node -> prev = list -> head -> prev;
  
  list -> head -> prev -> next = node;
  list -> head -> prev = node;
  list -> head = node;
}

void * list_retrieve(List * list, int index) {
  // retrieves the ith element in the list
  
  if (unlikely(index >= list -> size))
    exit_printing(ERROR_PROGRAMMER, "list_retrieve: index out of bounds");
  
  for (iterate(list, void *, entry))
    if ((int) entry_index == index)
      return entry;
  
  exit_printing(ERROR_PROGRAMMER, "list corruption observed when retrieving element %d", index);
}

void list_concat(List * first, List * other) {
  // concatenates two lists
  
  if (!other) return;    // nothing to concatenate
  else if (!first) {
    first = list_create();
    first -> head = other -> head;
    first -> size = other -> size;
    return;
  }
  else if (!first -> head) {
    first -> head = other -> head;
    first -> size = other -> size;
    return;
  }
  
  ListNode * other_tail = other -> head -> prev;
  
  first -> head -> prev -> next = other -> head;
  other -> head -> prev -> next = first -> head;
  other -> head -> prev         = first -> head -> prev;
  first -> head -> prev         = other_tail;
  
  first -> size += other -> size;
}

void list_remove(List * list, ListNode * node) {
  // removes a node from the list in constant time
  
  node -> next -> prev = node -> prev;   // Drop out of DLL
  node -> prev -> next = node -> next;   // ---------------
  
  if (node == list -> head)
    list -> head = node -> next;       // Advance head
  
  if (--list -> size == 0) list -> head = NULL;
  
  if (list -> value_free) (list -> value_free)(node -> value);
  free(node);
}

void list_empty(List * list) {
  // removes all size in the list
  
  while (list -> size)
    list_remove(list, list -> head);
}

void list_delete(List * list) {
  // completely deletes the list
  
  list_empty(list);
  free(list);
}
