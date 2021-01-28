
#include <stdio.h>
#include <stdlib.h>

#include "color.h"

#include "../structures/hashmap.h"

static Hashmap * colors_by_name;

void init_color() {
  
  colors_by_name = hashmap_create(hash_string, compare_strings, NULL, 16);
  
  hashmap_add(colors_by_name, "red"    , RED    );
  hashmap_add(colors_by_name, "green"  , GREEN  );
  hashmap_add(colors_by_name, "yellow" , YELLOW );
  hashmap_add(colors_by_name, "blue"   , BLUE   );
  hashmap_add(colors_by_name, "magenta", MAGENTA);
  hashmap_add(colors_by_name, "cyan"   , CYAN   );
  hashmap_add(colors_by_name, "gray"   , GRAY   );
}

void terminate_color() {
  hashmap_destroy(colors_by_name);
}

char * get_color_by_name(char * name) {
  return hashmap_get(colors_by_name, name);
}
