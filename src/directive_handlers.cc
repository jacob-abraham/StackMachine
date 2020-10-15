
#include "directive_handlers.h"


#define DIRECTIVE_HANDLER(NAME, rettype, ...) rettype DirectiveHandler_##NAME(__VA_ARGS__)

//simply validate the args as either TEXT or DATA
DIRECTIVE_HANDLER(SECTION, bool, std::string sectionTitle) {
  bool ret = false;
  ret = iStrEqual(sectionTitle, "TEXT") || iStrEqual(sectionTitle, "DATA");

  return ret;

}

//add "num" 0's to memory
DIRECTIVE_HANDLER(SPACE, void, WORD num, Memory* mem) {

  for(size_t i = 0; i < (size_t)num; i++) {
    mem->addData(0);
  }

}


//defualt handler
DIRECTIVE_HANDLER(UNKNOWN, void) {

}

#undef DIRECTIVE_HANDLER