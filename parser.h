/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"

class Parser {
  public:
    void parse_input();
    void readAndPrintAllInput();
  private:
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);
    void parse_tokens_section();
    void parse_token_list();
    void parse_token();
    void parse_expr();
};

#endif

