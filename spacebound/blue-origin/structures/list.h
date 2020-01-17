#pragma once

#include "../include/headers.h"

typedef struct ListNode {
  
  struct ListNode * next;
  struct ListNode * prev;
  
  void * value;
  
} ListNode;

typedef struct List {
  
  ListNode * head;       // the head of the list (has a value)
  ValueFree free;        // means by which nodes are free
  
  int size;              // number of elements in the list

  Mutex * lock;          // lock for when list is parallel
  
} List;

#define REP0(X)
#define REP1(X) X
#define REP2(X) REP1(X) X 
#define REP3(X)	REP2(X) X
#define REP4(X)	REP3(X) X
#define REP5(X)	REP4(X) X
#define REP6(X)	REP5(X) X
#define REP7(X)	REP6(X) X
#define REP8(X)	REP7(X) X
#define REP9(X)	REP8(X) X
#define REP(NUM, X) REP##NUM(X)
#define list_get(LIST, NUM) LIST -> head REP(NUM, -> next) -> value

#define iterate(LIST, TYPE, NAME)                                           \
  TYPE                                                                      \
  NAME             = (TYPE) *(int  *) &(LIST -> head),                      \
    * NAME##_node  =         (void *)  (LIST -> head),                      \
    * NAME##_index = 0;                                                     \
                                                                            \
  (int) NAME##_index < (LIST -> size) &&				    \
  ((NAME = (TYPE) *(int *) &((ListNode *) NAME##_node) -> value) || true);  \
                                                                            \
  NAME##_index   = ((void *) NAME##_index + 1),                             \
    NAME##_node  =  (void *) ((ListNode *) NAME##_node) -> next,            \
    NAME         = (TYPE) *(int *) &(NAME##_node)
