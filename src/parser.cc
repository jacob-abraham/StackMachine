
#include "parser.h"

void Parser::parse() {

  //initl mem
  memoryTable = new Memory(500,0,200);
  //std::string s(std::istreambuf_iterator<char>(input), {});
  //std::cout << s << std::endl;
  //std::cout << "Start" << lexer.getToken() << std::endl;
  parse_program();
}

Token Parser::expect(TokenType expected_type) {
  Token token = lexer.getToken();
  if(token.token_type != expected_type)
    throw SyntaxError("Unexpected token of type '" +
                             Token::getTokenType(expected_type) +
                             "' expected '" +
                             Token::getTokenType(token) +
                             "'", token.line_no);
  else
    return token;
}

#define PARSER_FUNC(rule, rettype, ...) \
rettype Parser::parse_##rule(__VA_ARGS__)


//program -> data_section text_section;
PARSER_FUNC(program, void) {
  bool hasDataSection = parse_data_section();
  parse_text_section(hasDataSection);
}
//data_section -> ;
//data_section -> directive data_list;
PARSER_FUNC(data_section, bool) {
  //data section begins ".section DATA"
  //if this is present parse a data list and return true

  Token t1 = lexer.getToken();
  Token t2 = lexer.getToken();
  Token t3 = lexer.getToken();
  lexer.ungetToken(t3);
  lexer.ungetToken(t2);
  lexer.ungetToken(t1);

  if(t1.token_type == DOT &&
     t2.token_type == ID &&
     iStrEqual(t2.lexeme, "SECTION") &&
     t3.token_type == ID &&
     iStrEqual(t3.lexeme, "DATA")) {
    //dont care about this directive, we already checked, just consumes
    auto* dir = parse_directive();
    delete dir;
    parse_data_list();
    return true;
  }

  return false;

}
//text_section -> directive text_list | text_list;
PARSER_FUNC(text_section, void, bool hasDataSection) {

  bool hasTextSectionDirective = false;
  { //in block for scoping
    Token t1 = lexer.getToken();
    Token t2 = lexer.getToken();
    Token t3 = lexer.getToken();
    lexer.ungetToken(t3);
    lexer.ungetToken(t2);
    lexer.ungetToken(t1);

    hasTextSectionDirective = t1.token_type == DOT &&
                              t2.token_type == ID &&
                              iStrEqual(t2.lexeme, "SECTION") &&
                              t3.token_type == ID &&
                              iStrEqual(t3.lexeme, "TEXT");
  }

  Token t = lexer.peek();

  //if it has a data section, must have a text directive
  if(hasDataSection) {
    if(!hasTextSectionDirective) {
      throw SyntaxError("If a DATA section is specified a TEXT section must be explicitly declared", t.line_no);
    }
    else {
      //dont care about this directive, we already checked, just consumes
      auto* dir = parse_directive();
      delete dir;
    }
  }

  //regardless, parse text_list
  parse_text_list();
}
//directive -> DOT ID directive_args;
PARSER_FUNC(directive, Directives::Directive*) {
  expect(DOT);
  Token t = expect(ID);
  auto dirType = Directives::determineDirective(t.lexeme);
  if(dirType == Directives::DirectiveType::kUNKNOWN) {
    throw SyntaxError("Invalid directive '"+t.lexeme+"'", t.line_no);
  }
  size_t nargs = Directives::getArgCount(dirType);
  auto args = parse_directive_args(nargs);
  auto* dir = new Directives::Directive;
  dir->type = dirType;
  dir->args = args;

  //before we return, call the handler
  bool handlerRet = false;
  //for special cases pass args
  switch(dirType) {
    case Directives::DirectiveType::kSECTION:
      handlerRet = DirectiveHandler_SECTION(args[0]);
      break;
    case Directives::DirectiveType::kSPACE:
      DirectiveHandler_SPACE(MemoryParser::getWord(args[0]), memoryTable);
      handlerRet = true;
      break;
    default:
      handlerRet = DirectiveHandler_SECTION(0);
      break;
  }

  if(!handlerRet) {
    throw SyntaxError("Bad directive handler", t.line_no);
  }

  return dir;
}
//directive_args -> ID | NUM;
PARSER_FUNC(directive_args, std::vector<std::string>, size_t nargs) {
  Token t;
  std::vector<std::string> args;
  for(size_t i = 0; i < nargs; i++) {

    t = lexer.getToken();
    if(t.token_type == NUM || t.token_type == ID) {
      args.push_back(t.lexeme);
    }
    else {
      throw SyntaxError("Invalid directive argument '"+t.lexeme+"'", t.line_no);
    }
  }
  return args;
}
//data_list -> data | data data_list;
PARSER_FUNC(data_list, void) {
  parse_data();
  Token t = lexer.peek();
  if(t.token_type == NUM ||
     t.token_type == AT ||
     t.token_type == DOT) {

    //check to make sure the next item is not a SECTION direction
    bool hasSectionDirective = false;
    { //in block for scoping
      Token t1 = lexer.getToken();
      Token t2 = lexer.getToken();
      lexer.ungetToken(t2);
      lexer.ungetToken(t1);

      hasSectionDirective = t1.token_type == DOT &&
                            t2.token_type == ID &&
                            iStrEqual(t2.lexeme, "SECTION");
    }

    //no section directive, more to go
    if(!hasSectionDirective) {
      parse_data_list();
    }
  }
}
//data -> directive | label | NUM;
PARSER_FUNC(data, void) {
  Token t = lexer.peek();
  if(t.token_type == NUM) {
    t = expect(NUM);
    //add to data
    WORD w = MemoryParser::getWord(t.lexeme);
    memoryTable->addData(w);
  }
  else if(t.token_type == AT) {
    LABEL label = parse_label();
    memoryTable->addDataLabel(label);
  }
  else if(t.token_type == DOT) {
    //TODO: for now throw away directive, eventually will need to handle this
    auto* dir = parse_directive();
    delete dir;
  }
  else {
    throw SyntaxError("Invalid DATA section element '"+t.lexeme+"'", t.line_no);
  }
}
//label -> AT ID
PARSER_FUNC(label, LABEL) {
  expect(AT);
  Token t = expect(ID);
  LABEL label = MemoryParser::getLabel(t.lexeme);
  return label;
}
//text_list -> text | text text_list;
PARSER_FUNC(text_list, void) {
  parse_text();
  Token t = lexer.peek();
  if(t.token_type == ID ||
     t.token_type == AT ||
     t.token_type == DOT) {


    //check to make sure the next item is not a SECTION direction
    bool hasSectionDirective = false;
    { //in block for scoping
      Token t1 = lexer.getToken();
      Token t2 = lexer.getToken();
      lexer.ungetToken(t2);
      lexer.ungetToken(t1);


      hasSectionDirective = t1.token_type == DOT &&
                            t2.token_type == ID &&
                            iStrEqual(t2.lexeme, "SECTION");
    }

    //no section directive, more to go
    if(!hasSectionDirective && lexer.peek().token_type != END_OF_FILE && lexer.peek().token_type != ERROR) {
      std::cout << lexer.peek().token_type << std::endl;
      parse_text_list();
    }
  }
}
//text -> inst | directive | label;
PARSER_FUNC(text, void) {
  std::cout << "t" << std::endl;
  Token t = lexer.peek();
  std::cout << "hello" << t.token_type << std::endl;
  if(t.token_type == ID) {
    parse_inst();
  }
  else if(t.token_type == AT) {
    LABEL label = parse_label();
    memoryTable->addTextLabel(label);
  }
  else if(t.token_type == DOT) {
    //TODO: for now throw away directive, eventually will need to handle this
    auto* dir = parse_directive();
    delete dir;
  }
  else {
    std::cout << t.token_type << std::endl;
    throw SyntaxError("Invalid TEXT section element '"+t.lexeme+"'", t.line_no);
  }
}
//inst -> opcode opcode_args
PARSER_FUNC(inst, void) {
  Token t = lexer.peek();
  if(t.token_type == ID) {
    Opcodes::OpcodeType op = parse_opcode();
    parse_opcode_args(Opcodes::getArgCount(op));
  }
  else {
    throw SyntaxError("Invalid instruction '"+t.lexeme+"'", t.line_no);
  }
}
//opcode -> ID;
PARSER_FUNC(opcode, Opcodes::OpcodeType) {
  Token t = expect(ID);
  Opcodes::OpcodeType op = Opcodes::determineOpcode(t.lexeme);
  if(op == Opcodes::OpcodeType::kUNKNOWN) {
    throw SyntaxError("Invalid opcode '"+t.lexeme+"'", t.line_no);
  }
  memoryTable->addText(op);
  return op;
}
//opcode_args -> NUM | label
//opcode_args -> ;
PARSER_FUNC(opcode_args, void, size_t nargs) {
  Token t;
  for(size_t i = 0; i < nargs; i++) {

    t = lexer.peek();
    if(t.token_type == NUM) {
      t = expect(NUM);
      IMMEDIATE imm = MemoryParser::getImmediate(t.lexeme);
      memoryTable->addText(imm);
    }
    else if(t.token_type == AT) {
      LABEL lbl = parse_label();
      memoryTable->addText(lbl);
    }
    else {
      throw SyntaxError("Invalid opcode argument '"+t.lexeme+"'", t.line_no);
    }
  }
}

#undef PARSER_FUNC