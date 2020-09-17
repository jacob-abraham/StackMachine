#include "opcodes.h"

char const* opcode_string_rep[] = {
  #define STRING_REP(name) #name,
    ALL_OPCODES(STRING_REP)
  #undef STRING_REP

  "Unknown"
};


char const* Opcodes::getString(OpcodeType type) {
  return opcode_string_rep[type];
}

#define CASE(name) case k##name:
#define DETERMINE_OP_TYPE(type, LIST)                                          \
bool Opcodes::is##type(OpcodeType t) {                                          \
  switch(t) {                                                                  \
    LIST(CASE)                                                                 \
      return true;                                                             \
    default:                                                                   \
      return false;                                                            \
  }                                                                            \
}

DETERMINE_OP_TYPE(Arithmetic, ARITHMETIC_OPS)
DETERMINE_OP_TYPE(Logical, LOGICAL_OPS)
DETERMINE_OP_TYPE(Bitwise, BITWISE_OPS)
DETERMINE_OP_TYPE(Memory, MEMORY_OPS)
DETERMINE_OP_TYPE(Stack, STACK_OPS)
DETERMINE_OP_TYPE(Flow, FLOW_OPS)

#undef DETERMINE_OP_TYPE
#undef CASE


//convert a string to a opcode
Opcodes::OpcodeType Opcodes::determineOpcode(std::string s) {
  #define CASE(op) if(iStrEqual(s, #op)) return OpcodeType::k##op;
    ALL_OPCODES(CASE)
  #undef CASE
  return OpcodeType::kUNKNOWN;
}

std::pair<size_t , size_t> Opcodes::getStackEffect(Opcodes::OpcodeType o) {
  switch(o) {
    case kADD:
    case kSUB:
    case kMUL:
    case kDIV:
    case kMOD:
      return {2,1};


    case kAND:
    case kBAND:
    case kOR:
    case kBOR:
      return {2,1};

    case kNEG:
    case kNOT:
    case kBNOT:
      return {1,1};

    case kBR_EQ:
    case kBR_NE:
      return {2,0};

    case kCOND:
      return {3,1};

    case kEQ:
    case kNE:
    case kGE:
    case kLE:
    case kGT:
    case kLT:
      return {2,1};

    case kLOAD:
      return {0,1};
    case kSTORE:
      return {1,0};

    case kPOP:
      return {1,0};
    case kPUSH:
      return {0,1};

    case kUNKNOWN:
    default:
      //error
      return {0,0};
  }
}

//get how many args this gets, whether label or num
size_t Opcodes::getArgCount(Opcodes::OpcodeType o) {

  switch(o) {

    case kADD:
    case kSUB:
    case kMUL:
    case kDIV:
    case kMOD:

    case kAND:
    case kBAND:
    case kOR:
    case kBOR:

    case kNEG:
    case kNOT:
    case kBNOT:

    case kCOND:

    case kEQ:
    case kNE:
    case kGE:
    case kLE:
    case kGT:
    case kLT:
      return 0;


    case kBR_EQ:
    case kBR_NE:
      return 1;

    case kLOAD:
    case kSTORE:
      return 1;

    case kPOP:
      return 0;
    case kPUSH:
      return 1;

    case kUNKNOWN:
    default:
      //error
      return 0;
  }
}