

#ifndef __DIRECTIVE_H__
#define __DIRECTIVE_H__



#include "stringhelper.h"
#include <vector>

//define listings of all opcodes

#define DATA_DIRECTIVE(F)                                                      \
F(SPACE)

#define SECTION_DIRECTIVE(F)                                                   \
F(SECTION)

#define ALL_DIRECTIVES(F)                                                      \
DATA_DIRECTIVE(F)                                                              \
SECTION_DIRECTIVE(F)

#define ALL_DIRECTIVE_TYPES(F)                                                 \
F(Data)                                                                        \
F(Section)                                                                     \


namespace Directives {

  //define enum of all directives
  enum DirectiveType {

    //create a list of all the directives
#define DirType(name) k##name,
    ALL_DIRECTIVES(DirType)
#undef DirType

    //calculate the number of directives by adding 1 for every opcode
    kUNKNOWN=0
        #define COUNT(...) +1
        ALL_DIRECTIVES(COUNT)
#undef COUNT

  };

  char const* getString(DirectiveType);

#define DETERMINE_TYPE_H(type) bool is##type(DirectiveType);
  ALL_DIRECTIVE_TYPES(DETERMINE_TYPE_H)
#undef DETERMINE_TYPE_H

  DirectiveType determineDirective(std::string);

  size_t getArgCount(DirectiveType);

  struct Directive {
    DirectiveType type;
    std::vector<std::string> args;
  };

};


#endif
