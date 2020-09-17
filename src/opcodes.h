#ifndef __OPCODES_H__
#define __OPCODES_H__

#include "stringhelper.h"
#include "memory.h"

//define listings of all opcodes

#define ARITHMETIC_OPS(F)                                                      \
F(ADD)                                                                         \
F(SUB)                                                                         \
F(MUL)                                                                         \
F(DIV)                                                                         \
F(MOD)                                                                         \
F(NEG)

#define LOGICAL_OPS(F)                                                         \
F(AND)                                                                         \
F(OR)                                                                          \
F(NOT)                                                                         \
F(EQ)                                                                          \
F(NE)                                                                          \
F(LT)                                                                          \
F(GT)                                                                          \
F(LE)                                                                          \
F(GE)

#define BITWISE_OPS(F)                                                         \
F(BAND)                                                                        \
F(BOR)                                                                         \
F(BNOT)

#define MEMORY_OPS(F)                                                          \
F(LOAD)                                                                        \
F(STORE)

#define STACK_OPS(F)                                                           \
F(PUSH)                                                                        \
F(POP)

#define FLOW_OPS(F)                                                            \
F(BR_EQ)                                                                       \
F(BR_NE)                                                                       \
F(COND)

#define ALL_OPCODES(F)                                                         \
ARITHMETIC_OPS(F)                                                              \
LOGICAL_OPS(F)                                                                 \
BITWISE_OPS(F)                                                                 \
MEMORY_OPS(F)                                                                  \
STACK_OPS(F)                                                                   \
FLOW_OPS(F)

#define ALL_OPCODE_TYPES(F)                                                    \
F(Arithmetic)                                                                  \
F(Logical)                                                                     \
F(Bitwise)                                                                     \
F(Memory)                                                                      \
F(Stack)                                                                       \
F(Flow)


namespace Opcodes {

  //define enum of all opcodes
  enum OpcodeType {

    //create a list of all the opcodes
    #define OPType(name) k##name,
      ALL_OPCODES(OPType)
    #undef OPType

    //calculate the number of opcodes by adding 1 for every opcode
    kUNKNOWN=0
    #define COUNT(...) +1
      ALL_OPCODES(COUNT)
    #undef COUNT

  };

  char const* getString(OpcodeType);

  #define DETERMINE_OP_TYPE_H(type) bool is##type(OpcodeType);
    ALL_OPCODE_TYPES(DETERMINE_OP_TYPE_H)
  #undef DETERMINE_OP_TYPE_H

  OpcodeType determineOpcode(std::string);

  std::pair<size_t , size_t> getStackEffect(OpcodeType);
  size_t getArgCount(OpcodeType);


  struct OpcodeArg {
    enum OpcodeArgType {
      kIMMEDIATE=0,kLABEL,kADDRESS
    };
    OpcodeArgType type;
    int64_t value;
  };
  typedef std::vector<Opcodes::OpcodeArg> OpcodeArgs;

  struct Opcode {
    OpcodeType op;
    OpcodeArgs args;
  };

};




#endif
