
#include "../include/program.h"

ListNode * list_create_node(void * value) {
  ListNode * node = malloc(sizeof(ListNode));
  node -> value = value;
  node -> next = NULL;
  node -> prev = NULL;
  return node;
}

List * list_create() {
  // Creates a list with potentially limited length
  
  List * list = malloc(sizeof(List));
  list -> head = NULL;
  list -> free = NULL;
  list -> lock = NULL;
  list -> size = 0;
  return list;
}

List * list_from(int vargs, ...) {
  // Creates a list from arguments
  // The list returned is an SLL with no auto freeing

  List * list = list_create();
  
  va_list argPointer;
  
  va_start(argPointer, vargs);
  
  for (int i = 0; i < vargs; i++) {
    list_insert(list, va_arg(argPointer, void *));
  }
  
  va_end(argPointer);
  
  return list;
}

List * list_that_frees(ValueFree freer) {
  List * list = list_create();
  list -> free = freer;
  return list;
}

void list_lock(List * list) {

  if (!list -> lock) {
    list -> lock = malloc(sizeof(Mutex));
    pthread_mutex_init(list -> lock, NULL);
  }
  
  pthread_mutex_lock(list -> lock);
}

void list_unlock(List * list) {
  pthread_mutex_unlock(list -> lock);
}

void list_insert(List * list, void * value) {
  // Inserts node into the linked list
  // Complexity: O(1)

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
  // Inserts node into the linked list
  // Complexity: O(1)

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
  // Removes node from list
  
  node -> next -> prev = node -> prev;   // Drop out of DLL
  node -> prev -> next = node -> next;   // ---------------
  
  if (node == list -> head) {
    list -> head = node -> next;       // Advance head
  }
  
  if (--list -> size == 0) list -> head = NULL;

  if (list -> free) (list -> free)(node -> value);
  free(node);
}

void list_empty(List * list) {
  // Removes all size in the list
  
  while (list -> size) {
    list_remove(list, list -> head);
  }
}

void list_destroy(List * list) {
  // Completely destroys the list
  
  if (list -> lock) {
    pthread_mutex_destroy(list -> lock);
    free(list -> lock);
    list -> lock = NULL;
  }
  
  list_empty(list);
  free(list);
}
