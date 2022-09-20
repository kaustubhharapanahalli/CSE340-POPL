/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef LEXER_H_
#define LEXER_H_

#include <string>
#include <vector>

#include "./inputbuf.h"

// ------- token types -------------------

typedef enum {
  END_OF_FILE = 0,
  LPAREN,
  RPAREN,
  HASH,
  ID,
  COMMA,
  DOT,
  STAR,
  OR,
  UNDERSCORE,
  SYMBOL,
  CHAR,
  INPUT_TEXT,
  ERROR
} TokenType;

class Token {
 public:
  void Print();

  std::string lexeme;
  TokenType token_type;
  int line_no;
};

class LexicalAnalyzer {
 public:
  Token GetToken();
  Token peek(int);
  LexicalAnalyzer();

 private:
  std::vector<Token> tokenList;
  Token GetTokenMain();
  int line_no;
  int index;
  Token tmp;
  InputBuffer input;

  bool SkipSpace();
  Token ScanIdOrChar();
  Token ScanSymbol();
  Token ScanInput();
};

#endif  // LEXER_H_
