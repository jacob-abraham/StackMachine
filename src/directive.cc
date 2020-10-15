
#include "directive.h"


char const* directive_string_rep[] = {
#define STRING_REP(name) #name,
    ALL_DIRECTIVES(STRING_REP)
#undef STRING_REP

    "Unknown"
};


char const* Directives::getString(DirectiveType type) {
  return directive_string_rep[type];
}

#define CASE(name) case k##name:
#define DETERMINE_TYPE(type, LIST)                                             \
bool Directives::is##type(DirectiveType t) {                                    \
  switch(t) {                                                                  \
    LIST(CASE)                                                                 \
      return true;                                                             \
    default:                                                                   \
      return false;                                                            \
  }                                                                            \
}

DETERMINE_TYPE(Data, DATA_DIRECTIVE)
DETERMINE_TYPE(Section, SECTION_DIRECTIVE)

#undef DETERMINE_OP_TYPE
#undef CASE



//convert a string to a opcode
Directives::DirectiveType Directives::determineDirective(std::string s) {
#define CASE(x) if(iStrEqual(s, #x)) return DirectiveType::k##x;
  ALL_DIRECTIVES(CASE)
#undef CASE
  return DirectiveType::kUNKNOWN;
}


//get how many args this gets, whether label, id, or num
size_t Directives::getArgCount(Directives::DirectiveType d) {

  switch(d) {
#define CASE(x) case k##x:
    SECTION_DIRECTIVE(CASE)
#undef CASE
      return 1;
#define CASE(x) case k##x:
    DATA_DIRECTIVE(CASE)
#undef CASE
      return 1;
    case kUNKNOWN:
    default:
      //error
      return 0;
  }
}
