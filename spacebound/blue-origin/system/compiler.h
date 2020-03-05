#pragma once

/* A nicer name for state that's local to a     *
 * file. A .c file will often start with local  *
 * declarations meant only for that file to     *
 * use. See color.c for a simple example.       *
 *                                              */
#define local static

/* Length of an array                           */
#define len(ARRAY)                              \
  (sizeof(ARRAY) / sizeof(ARRAY[0]))

/* An alternative control-flow in for-loops:    *
 *                                              *
 *     for (iterate(names, char *, name)) {     *
 *                                              *
 *         when (strlen(name) < 4);             *
 *                                              *
 *         // now you know 'name' is short.     *
 *         // do work with that assumption.     *
 *     }                                        *
 *                                              */
#define when(condition)                         \
  if (!(condition)) continue


/* When you know how a condition will evaluate  *
 * over 99.9% of the time, the following macros *
 * can inform the compiler of your knowledge.   *
 * This comes from the Linux Kernel's source.   *
 *                                              *
 * you use it around a conditional, as below:   *
 *                                              *
 *    if (unlikely(file_status < 0)) {          *
 *        // do rare event                      *
 *    }                                         *
 *                                              */
#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)


/* Adds flags for optimization. These come from *
 * the gcc manual, which should be consulted    *
 * before use, since many require very specific *
 * knowledge about the program. This macro is   *
 * used in function declerations, as below:     *
 *                                              *
 *     extern int square(int) opt(const);       *
 *     extern int quit()      opt(naked, near)  *
 *                                              */
#define opt(COMPILER_FLAG, ...)                 \
  __attribute__((COMPILER_FLAG, ##__VA_ARGS__))


/* Call function as though it were a method.    *
 * This is useful for structs that contain      *
 * function pointers, as seen in the equivalent *
 * statements below:                            *
 *                                              *
 *     $(ds32, read, true)                      *
 *                                              *
 *     (ds32 -> read)(ds32, read, true)         *
 *                                              */
#define $(CLASS, METHOD, ...)                   \
  ((CLASS) -> METHOD)(CLASS, ##__VA_ARGS__)


/* Make double-frees unlikely. The idea is that *
 * we don't want to use a pointer after it's    *
 * been free'd.                                 *
 *                                              */
#define blank(pointer)                          \
  ({                                            \
    free(pointer);                              \
    pointer = NULL;                             \
  })


/* Protects the critical section specified in   *
 * the body. 'break' allows the section to      *
 * leave with the mutex, as seen below:         *
 * It's used as follows:                        *
 *                                              *
 *     aquire(network_mutex) {                  *
 *                                              *
 *         // do stuff safely                   *
 *                                              *
 *         if (still_need_lock)                 *
 *             break;                           *
 *     }                                        *
 *                                              */
#define aquire(LOCK)                            \
  for (int AQUIRED_ = pthread_mutex_lock(LOCK); \
       !AQUIRED_;                               \
       AQUIRED_ = 1, pthread_mutex_unlock(LOCK))
