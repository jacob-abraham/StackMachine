#ifndef __PARSER_H__
#define __PARSER_H__

#include <vector>
#include <iostream>
#include <ostream>
#include <sstream>
#include <exception>
#include <cstdlib>
#include <stdint.h>
#include "stringhelper.h"
#include "lexer.h"
#include "opcodes.h"
#include "directive.h"
#include "directive_handlers.h"
#include "memory.h"

typedef std::vector<std::string> StringList;

class SyntaxError: public std::exception {
public:
  std::string message;
  int line_no;

  SyntaxError(std::string msg, int ln) : message(msg), line_no(ln) {}

  SyntaxError() : message("Unknown"), line_no(-1) {}

  virtual const char* what() const throw()
  {
    //const ref trick to allow for copy so string isnt local stack memory
    const std::string& ret = getString();
    return ret.c_str();
  }

  friend std::ostream& operator<<(std::ostream& out, const SyntaxError& se) {
    out << se.getString();
    return out;
  }
  std::string getString() const {
    std::stringstream str;
    str << "Syntax Error: " << this->message << " on line " << this->line_no;
    return str.str();
  }
  std::string getString() {
    return const_cast<const SyntaxError*>(this)->getString();
  }
};


class Parser {

  //public interface
public:

  explicit Parser(std::istream& in) : lexer(in), memoryTable(NULL) {}

  explicit Parser(Lexer& lexer) : lexer(lexer), memoryTable(NULL) {}

  void parse();
  Memory* getMemory() {return memoryTable;}


private:
  Lexer lexer;
  Memory* memoryTable;

  Token expect(TokenType);


#define PARSER_RULES(F)                                                        \
F(program, void)                                                               \
F(data_section, bool)                                                          \
F(text_section, void, bool)                                                    \
F(directive, Directives::Directive*)                                         \
F(directive_args, std::vector<std::string>, size_t)                                                        \
F(data_list, void)                                                             \
F(data, void)                                                                  \
F(label, LABEL)                                                                \
F(text_list, void)                                                             \
F(text, void)                                                                  \
F(inst, void)                                                                  \
F(opcode, Opcodes::OpcodeType)                                                 \
F(opcode_args, void, size_t)

#define HEADER(rule, rettype, ...) rettype parse_##rule(__VA_ARGS__);
  PARSER_RULES(HEADER)
#undef PASRER_HEADER

};

#endif

