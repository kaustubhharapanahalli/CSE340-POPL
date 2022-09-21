/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef PARSER_H_
#define PARSER_H_

#include <string>

#include "./lexer.h"

class Parser {
 public:
  void parseInput();
  void parse_input();
  void readAndPrintAllInput();

 private:
  LexicalAnalyzer lexer;
  void syntax_error();
  void expression_syntax_error(Token token_id);
  void semantic_error();
  Token expect_expr(TokenType expected_type, Token token_id);
  Token expect(TokenType expected_type);
  void parse_tokens_section();
  void parse_token_list();
  void parse_token();
  void parse_expr(Token token_id);
};

#endif  // PARSER_H_
