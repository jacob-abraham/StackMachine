
#ifndef __DIRECTIVE_HANDLERS__
#define __DIRECTIVE_HANDLERS__

#include "directive.h"
#include "stringhelper.h"
#include "memory.h"
#include <cstdarg>


#define DIRECTIVE_HANDLER(NAME) bool DirectiveHandler_##NAME(size_t nargs __attribute__((unused)),...)

//simply validate the args as either TEXT or DATA
DIRECTIVE_HANDLER(SECTION) {
  std::va_list args;
  va_start(args, nargs);
  bool ret = false;

  if(nargs == 1) {
    std::string* sectionTitle = va_arg(args, std::string*);
    ret = iStrEqual(*sectionTitle, "TEXT") || iStrEqual(*sectionTitle, "DATA");
  }

  va_end(args);
  return ret;

}

//add "num" 0's to memory
DIRECTIVE_HANDLER(SPACE) {
  std::va_list args;
  va_start(args, nargs);
  bool ret = false;

  if(nargs == 2) {
    ret = true;
    size_t num = va_arg(args, size_t);
    Memory* mem = va_arg(args, Memory*);
    for(size_t i = 0; i < num; i++) {
      mem->addData(0);
    }
  }

  va_end(args);
  return ret;
}


//defualt handler
DIRECTIVE_HANDLER(UNKNOWN) {
  return false;
}

#undef DIRECTIVE_HANDLER


#endif
