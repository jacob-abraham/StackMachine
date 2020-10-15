#ifndef __WASM_BINDINGS__
#define __WASM_BINDINGS__

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <exception>

#include "parser.h"
#include "lexer.h"
#include "memory.h"


/*Lexer* build_Lexer(std::string s) {
  std::istringstream iss(s);
  std::cout << "First: " << (int)s[0] << s[0] << std::endl;
  std::cout << s << std::endl;
  std::cout << "Done" << std::endl;
  Lexer* l = new Lexer(iss);
  std::cout << l->getToken() << l->getToken() << l->getToken() << std::endl;
  return l;
}*/
Parser* build_Parser(std::string s) {
  s += '\0';
  std::stringstream iss(s);
  ///td::cout << &iss << std::endl;
  //std::cout << s << std::endl;
  //std::cout << "First: " << (int)s[0] << s[0] << std::endl;
  Parser* p = new Parser(iss);
  return p;
}
void _test(std::string s) {
  std::stringstream iss(s);
  Parser p(iss);
  p.parse();
}
std::string getExceptionMessage(uintptr_t ptr) {
  return std::string(reinterpret_cast<std::exception *>(ptr)->what());
}


// Binding code
EMSCRIPTEN_BINDINGS(StackMachineModule) {

  //register helper funcs
  emscripten::function("build_Parser", &build_Parser, emscripten::allow_raw_pointers());
  //emscripten::function("build_Lexer", &build_Lexer, emscripten::allow_raw_pointers());
  emscripten::function("getExceptionMessage", &getExceptionMessage);
   emscripten::function("test", &_test);


  //register Lexer
  /*emscripten::class_<Lexer>("Lexer")
    .function("getToken", &Lexer::getToken)
    .function("peek", &Lexer::peek)
    .function("ungetToken", &Lexer::ungetToken)
    ;*/

  //register Token
  /*emscripten::value_object<Token>("Token")
    //.function("getString", &Token::getString)
    .field("lexeme",&Token::lexeme);
    ;*/

  //register Parser
  emscripten::class_<Parser>("Parser")
    .constructor(&build_Parser,emscripten::allow_raw_pointers())
    .function("parse", &Parser::parse, emscripten::allow_raw_pointers())
    .function("getMemory", &Parser::getMemory, emscripten::allow_raw_pointers())
    ;

  //register Memory
  emscripten::class_<Memory>("Memory")
    .function("getDataString", &Memory::getDataString)
    .function("getDataString", &Memory::getDataString)
    .function("getMemString", &Memory::getMemString)
    .function("getMemSectionString", &Memory::getMemSectionString)
    ;

  //register types
  emscripten::register_vector<std::string>("vector<string>");
}
#endif

#endif
