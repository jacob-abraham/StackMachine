
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "lexer.h"
#include "parser.h"
#include <map>
#include <fstream>
#include "opcodes.h"
#include "memory.h"


int main() {

  std::fstream in("simple.txt");

  Parser p(in);
  try {
    p.parse();
    Memory* mem = p.getMemory();
    std::cout << mem->getDataString() << std::endl;
    std::cout << mem->getTextString() << std::endl;
  }
  catch(SyntaxError se) {
    std::cout << "Error" << se << std::endl;
  }
  in.close();

  //std::cout << mem->getMemString() << std::endl;

  return 0;
}

