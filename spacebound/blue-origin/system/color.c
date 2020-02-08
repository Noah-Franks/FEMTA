
#include "../include/program.h"

local Hashmap * colors_by_name;    // map of name strings to color codes

void init_color() {
  
  colors_by_name = hashmap_create(hash_string, compare_strings, NULL, NULL, 16);
  
  hashmap_add(colors_by_name, "white"  , RESET  );
  hashmap_add(colors_by_name, "red"    , RED    );
  hashmap_add(colors_by_name, "green"  , GREEN  );
  hashmap_add(colors_by_name, "yellow" , YELLOW );
  hashmap_add(colors_by_name, "blue"   , BLUE   );
  hashmap_add(colors_by_name, "magenta", MAGENTA);
  hashmap_add(colors_by_name, "purple" , PURPLE );
  hashmap_add(colors_by_name, "cyan"   , CYAN   );
  hashmap_add(colors_by_name, "grey"   , GREY   );
  hashmap_add(colors_by_name, "gray"   , GRAY   );
  hashmap_add(colors_by_name, "orange" , ORANGE );
  hashmap_add(colors_by_name, "pink"   , PINK   );
  hashmap_add(colors_by_name, "mint"   , MINT   );
  hashmap_add(colors_by_name, "maroon" , MAROON );
  hashmap_add(colors_by_name, "salmon" , SALMON );
  hashmap_add(colors_by_name, "brown"  , BROWN  );
}

void drop_color() {
  hashmap_delete(colors_by_name);
  colors_by_name = NULL;
}

char * get_color_by_name(char * name) {
  return hashmap_get(colors_by_name, name);
}
