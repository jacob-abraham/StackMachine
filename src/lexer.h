#ifndef __LEXER_H__
#define __LEXER_H__

#include <vector>
#include <string>
#include <istream>
#include <iostream>
#include <ostream>


#define LEXER_TOKENS(F)                                                        \
F(NUM)                                                                         \
F(ID)                                                                          \
F(DOT)                                                                         \
F(AT)                                                                          \
F(NEWLINE)                                                                     \
F(SEMICOLON)

enum TokenType {

  END_OF_FILE = 0,
#define TOKEN(t) t,
LEXER_TOKENS(TOKEN)
#undef TOKEN
  ERROR
};

class Token {
public:

  std::string lexeme;
  TokenType token_type;
  int line_no;

  Token(std::string lexeme, TokenType token_type, int line_no) : lexeme(lexeme), token_type(token_type),
                                                                 line_no(line_no) {}

  Token() : lexeme(), token_type(ERROR), line_no(-1) {}

  static const std::string types[];

  friend std::ostream &operator<<(std::ostream &out, const Token &t) {
    out << "{" << Token::types[t.token_type] << ",";
    if (!t.lexeme.empty()) out << t.lexeme << ",";
    out << t.line_no << "}";
    return out;
  }

  static std::string getTokenType(const Token &t) { return Token::types[t.token_type]; }

  static std::string getTokenType(const TokenType &tt) { return Token::types[tt]; }
};

class Lexer {
public:
  Token getToken();

  Token peek();

  TokenType ungetToken(Token);

  explicit Lexer(std::istream &input) : input(input), line_number(1), tokens(), input_buffer() {}

  Lexer(const Lexer &old) : input(old.input), line_number(old.line_number), tokens(old.tokens),
                            input_buffer(old.input_buffer) {}

private:
  std::istream &input;
  int line_number;
  std::vector<Token> tokens;
  std::vector<char> input_buffer;


  //get ID
  Token getID();

  bool isIDStart();

  bool isIDBody();

  //returns NUM
  Token getNUM();
  bool isNUM();

  //get symbols
  Token getSymbol();

  bool isSymbol();


  //skip stuff
  bool isComment();

  void ignoreComment();

  void skipWhiteSpace();

  //input buffer code
  bool endOfInput();

  char peekChar();

  void ungetChar(char);

  char getChar();
};

#endif
