/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include "./parser.h"

#include <cstdlib>
#include <iostream>

struct token_information {
  /* data */
};

// The below set of code is to read the input provided in the Token section

void Parser::syntax_error() {
  std::cout << "SYNTAX ERROR\n";
  exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type) {
  Token t = lexer.GetToken();
  if (t.token_type != expected_type) syntax_error();
  return t;
}

// This function simply reads and prints all tokens
// I included it as an example. You should compile the provided code
// as it is and then run ./a.out < tests/test0.txt to see what this function
// does
void Parser::readAndPrintAllInput() {
  Token t;

  // get a token
  t = lexer.GetToken();

  // while end of input is not reached
  while (t.token_type != END_OF_FILE) {
    t.Print();             // pringt token
    t = lexer.GetToken();  // and get another one
  }

  // note that you should use END_OF_FILE and not EOF
}

void Parser::parse_input() {
  parse_tokens_section();
  // TODO(kaustubh): myLexicalAnalysis to be added here.
  Token t = expect(INPUT_TEXT);

  expect(END_OF_FILE);
}

void Parser::parse_tokens_section() {
  Token t;
  parse_token_list();
  t = expect(HASH);
}

void Parser::parse_token_list() {
  parse_token();
  Token t = lexer.peek(1);
  if (t.token_type == COMMA) {
    expect(COMMA);
    parse_token_list();
  } else if (t.token_type == HASH) {
    return;
  } else {
    syntax_error();
  }
}

void Parser::parse_token() {
  Token token_id = expect(ID);
  parse_expr(token_id);
}

void Parser::parse_expr(Token token_id) {
  Token t = lexer.peek(1);

  if ((t.token_type == CHAR)) {
    expect(CHAR);
    Token t = lexer.peek(1);
    if (t.token_type == END_OF_FILE) {
      return;
    } else if (t.token_type == RPAREN) {
      expect(RPAREN);
      parse_expr(token_id);
    } else {
      syntax_error();
    }
  } else if (t.token_type == LPAREN) {
    expect(LPAREN);
    parse_expr(token_id);
    expect(RPAREN);

  } else if (t.token_type == UNDERSCORE) {
  } else {
    std::cout << "SYNTAX ERROR IN EXPRESSION OF " << token_id.token_type
              << std::endl;
    exit(1);
  }
}

int main() {
  // note: the parser class has a lexer object instantiated in it (see file
  // parser.h). You should not be declaring a separate lexer object.
  // You can access the lexer object in the parser functions as shown in
  // the example  method Parser::readAndPrintAllInput()
  // If you declare another lexer object, lexical analysis will
  // not work correctly
  Parser parser;
  // parser.readAndPrintAllInput();
  parser.parse_input();
}
