#pragma once

#define local static

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

#define opt(COMPILER_FLAG, ...)                 \
  __attribute__((COMPILER_FLAG, ##__VA_ARGS__))

#define $(CLASS, METHOD, ...)                   \
  ((CLASS) -> METHOD)(CLASS, ##__VA_ARGS__)

#define blank(pointer)                          \
  ({                                            \
    free(pointer);                              \
    pointer = NULL;                             \
  })

#define when(condition)                         \
  if (!(condition)) continue

#define except(condition)                       \
  if (condition) break

#define aquire(LOCK)                                                    \
  for (int FINISHED = pthread_mutex_lock(LOCK); !FINISHED; FINISHED = 1, pthread_mutex_unlock(LOCK))
