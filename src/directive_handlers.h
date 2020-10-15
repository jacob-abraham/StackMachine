
#ifndef __DIRECTIVE_HANDLERS__
#define __DIRECTIVE_HANDLERS__

#include "directive.h"
#include "stringhelper.h"
#include "memory.h"

#define DIRECTIVE_HANDLER_H(NAME, rettype, ...) rettype DirectiveHandler_##NAME(__VA_ARGS__);

DIRECTIVE_HANDLER_H(SECTION, bool, std::string);
DIRECTIVE_HANDLER_H(SPACE, void, WORD, Memory*);
DIRECTIVE_HANDLER_H(UNKNOWN, void);

#undef DIRECTIVE_HANDLER_H

#endif
