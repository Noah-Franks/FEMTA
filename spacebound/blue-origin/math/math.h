#pragma once

#define square(X)				\
  ({ __typeof__(X) __x = (X);			\
    __x * __x;					\
  })

#define cube(X)					\
  ({ __typeof__(X) __x = (X);			\
    __x * __x * __x;				\
  })

#define between(a, b, c)                                                \
  ({ __typeof__ (b) __b = (b);                                          \
    ((a) <= (__b) && ((__b) <= (c)));                                   \
  })
