#ifndef HEADER_GAURD_CLASS
#define HEADER_GAURD_CLASS

#define $(CLASS, METHOD, ...)			\
  ((CLASS) -> METHOD)(CLASS, ##__VA_ARGS__)

#endif
