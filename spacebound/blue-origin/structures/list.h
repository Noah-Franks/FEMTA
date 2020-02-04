#pragma once

#include "../include/headers.h"

typedef struct ListNode {
  
  struct ListNode * next;    // the node after this one
  struct ListNode * prev;    // the node before this one
  
  void * value;              // the content this node holds
  
} ListNode;

typedef struct List {
  
  ListNode * head;           // the head of the list (same as every other nodes)
  freer      value_free;     // means by which nodes are free'd
  
  int elements;              // the number of elements in the list
  
} List;

// repetition macros, which enables the compiler-optimizable list_get macro
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

/* The following macro iterates over the elements in a list.                *
 * It provides an index which can be used to count through the elements,    *
 * and exposes the list node so that you can modify the list while          *
 * iterating. That's right, you can edit the list while iterating!          *
 * The TYPE argument can be any C type.                                     *
 *                                                                          *
 * You use the macro as a for-loop's expression, as seen below:             *
 *                                                                          *
 *   for (iterate(string_list, char *, string)) {                           *
 *      int index = (int) string_index;                                     *
 *      ListNode * node = (ListNode *) string_node;                         *
 *      printf("%d: %p %s\n", index, node, string);                         *
 *   }                                                                      *
 *                                                                          */
#define iterate(LIST, TYPE, NAME)                                           \
  TYPE                                                                      \
  NAME             = (TYPE) *(int  *) &(LIST -> head),                      \
    * NAME##_node  =         (void *)  (LIST -> head),                      \
    * NAME##_index = 0;                                                     \
                                                                            \
  (int) NAME##_index < (LIST -> elements) &&                                \
  ((NAME = (TYPE) *(int *) &((ListNode *) NAME##_node) -> value) || true);  \
                                                                            \
  NAME##_index   = ((void *) NAME##_index + 1),                             \
    NAME##_node  =  (void *) ((ListNode *) NAME##_node) -> next,            \
    NAME         = (TYPE) *(int *) &(NAME##_node)
