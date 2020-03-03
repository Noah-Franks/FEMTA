
#include "../include/program.h"

local Hashmap * color_codes;               // maps names to bash color codes

void init_console() {                        // populate the color code hashmap
  
  color_codes = hashmap_create(hash_string, compare_strings, NULL, NULL, 16);
  
  hashmap_add(color_codes, "white"  , RESET  );
  hashmap_add(color_codes, "red"    , RED    );
  hashmap_add(color_codes, "green"  , GREEN  );
  hashmap_add(color_codes, "yellow" , YELLOW );
  hashmap_add(color_codes, "blue"   , BLUE   );
  hashmap_add(color_codes, "magenta", MAGENTA);
  hashmap_add(color_codes, "purple" , PURPLE );
  hashmap_add(color_codes, "cyan"   , CYAN   );
  hashmap_add(color_codes, "grey"   , GREY   );
  hashmap_add(color_codes, "gray"   , GRAY   );
  hashmap_add(color_codes, "orange" , ORANGE );
  hashmap_add(color_codes, "pink"   , PINK   );
  hashmap_add(color_codes, "mint"   , MINT   );
  hashmap_add(color_codes, "maroon" , MAROON );
  hashmap_add(color_codes, "salmon" , SALMON );
  hashmap_add(color_codes, "brown"  , BROWN  );
}

void drop_console() {                        // delete the color code hashmap
  hashmap_delete(color_codes);
  color_codes = NULL;
}

char * get_color_by_name(char * name) {    // retrieves a color code
  return hashmap_get(color_codes, name);
}
