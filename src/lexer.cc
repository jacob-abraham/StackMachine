//
// Created by Jacob Abraham on 8/28/20.
//

#include "lexer.h"

const std::string Token::types[] = {
    "END_OF_FILE",
#define TOKEN(t) #t,
    LEXER_TOKENS(TOKEN)
#undef TOKEN
    "ERROR"
};


Token Lexer::getToken() {
  //if we have tokens, get them,
  if(!tokens.empty()) {
    Token t = tokens.back();
    tokens.pop_back();
    return t;
  }

  /*std::cout << &input << std::endl;
  std::string s;
  input >> s;
  std::cout << s << std::endl;

  std::cout << "finding token" << std::endl;*/


  //skip all spaces
  skipWhiteSpace();

  //ignore comments
  while(isComment()) {
    ignoreComment();
    //after we ignore a comment, skip all whitespace
    skipWhiteSpace();
  }

  //check for each element
  if(isIDStart()) return getID();
  if(isNUM()) return getNUM();
  if(isSymbol()) return getSymbol();

  std::cout << "c: " << (unsigned int)peekChar() << "fail to find token" << line_number << std::endl;

  //if at this point we have not returned, its either eof or error
  Token t("", TokenType::ERROR, line_number);
  if(endOfInput()) {
    std::cout << "EOF" << std::endl;
    std::cout << "c: " << (unsigned int)getChar() << "fail to find token" << line_number << std::endl;
    std::cout << "c: " << (unsigned int)getChar() << "fail to find token" << line_number << std::endl;
    std::cout << "c: " << (unsigned int)getChar() << "fail to find token" << line_number << std::endl;
    t.token_type = TokenType::END_OF_FILE;
  }
  return t;
}

Token Lexer::peek() {
  Token t = getToken();
  ungetToken(t);
  return t;
}

TokenType Lexer::ungetToken(Token t) {
  tokens.push_back(t);
  return t.token_type;
}

//returns ID
Token Lexer::getID() {
  Token t("", TokenType::ERROR, line_number);

  if(isIDStart()) {
    //get the start token
    t.lexeme += getChar();
    //get the rest of the num
    while(!endOfInput() && isIDBody()) t.lexeme += getChar();
    //now we have a valid token
    t.token_type = TokenType::ID;
  }
  return t;
}

bool Lexer::isIDStart() {
  char c = peekChar();
  return isalpha(c) || c == '_';
}

bool Lexer::isIDBody() {
  char c = peekChar();
  return isalnum(c) || c == '_';
}

//returns NUM
Token Lexer::getNUM() {
  Token t("", TokenType::ERROR, line_number);

  while(!endOfInput() && isNUM()) t.lexeme += getChar();
  //now we have a valid token
  t.token_type = TokenType::NUM;

  return t;
}
bool Lexer::isNUM() {
  char c = peekChar();
  return isdigit(c);
}

Token Lexer::getSymbol() {
  Token t("", TokenType::ERROR, line_number);

  //get the char
  if(isSymbol()) {
    char c = getChar();
    switch(c) {
      case '.': {
        t.token_type = TokenType::DOT;
        break;
      }
      case '@': {
        t.token_type = TokenType::AT;
        break;
      }
    }
  }
  return t;
}

bool Lexer::isSymbol() {
  char c = peekChar();
  return c == '.' || c == '@';
}


void Lexer::skipWhiteSpace() {
  //get a new char
  char c = getChar();
  //while not eof and c is whitespace
  while(!endOfInput() && isspace(c)) c = getChar();
  //if we got one too many, put it back
  if(!endOfInput()) ungetChar(c);

}

bool Lexer::isComment() {
  char c = peekChar();
  return c == '#';
}

void Lexer::ignoreComment() {

  //get a new char
  char c = getChar();
  //while not eof and not end of line
  while(!endOfInput() && c != '\n') c = getChar();
}

bool Lexer::endOfInput() {
  if(!input_buffer.empty()) {
    //make sure input buffer isnt just holding eof
    if(input_buffer.front() == '\0') return true;
    else return false;
  }
  else return input.eof();
}

char Lexer::peekChar() {
  char c = getChar();
  ungetChar(c);
  return c;
}

void Lexer::ungetChar(char c) {
  if(c != EOF) input_buffer.push_back(c);
  if(c == '\n') line_number--;
}

char Lexer::getChar() {

  //check for end of input
  if(endOfInput()) return '\0';

  char c;
  if(!input_buffer.empty()) {
    c = input_buffer.back();
    input_buffer.pop_back();
  }
  else input.get(c);

  if(c == '\n') line_number++;

  return c;
}
