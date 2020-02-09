#pragma once

// sees if b belongs to [a, c]
#define between(a, b, c)                \
  ({ __typeof__ (b) __b = (b);          \
    ((a) <= (__b) && ((__b) <= (c)));   \
  })
