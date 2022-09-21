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
#include <vector>

// Definition of vector to track token values to assist in raising expression
// syntax error when required.
std::vector<Token> track_token;
std::vector<std::string> track_semantic_error_messages;

// The below set of code is to read the input provided in the Token section

/*
 *  Functions provides a simplified way of throwing syntax error.
 */
void Parser::syntax_error() {
  std::cout << "SNYTAX ERORR" << std::endl;
  exit(1);
}

void Parser::expression_syntax_error(Token token_id) {
  std::cout << "SYNTAX ERROR IN EXPRESSION OF " + token_id.lexeme << std::endl;
  exit(1);
}

void Parser::semantic_error() {
  if (~track_semantic_error_messages.empty()) {
    for (int i = 0; i < static_cast<int>(track_semantic_error_messages.size());
         i++) {
      std::cout << track_semantic_error_messages[i] << std::endl;
    }
    exit(1);
  }
  return;
}

/*
 * this function gets a token and checks if it is
 * of the expected type. If it is, the token is
 * returned, otherwise, synatx_error() is generated
 * this function is particularly useful to match
 * terminals in a right hand side of a rule.
 * Written by Mohsen Zohrevandi
 */
Token Parser::expect(TokenType expected_type) {
  Token token_object = lexer.GetToken();
  if (token_object.token_type != expected_type) syntax_error();
  return token_object;
}

Token Parser::expect_expr(TokenType expected_type, Token token_id) {
  Token token_object = lexer.GetToken();

  if (token_object.token_type != expected_type) {
    expression_syntax_error(token_id);
  }
  return token_object;
}

/*
 * This function simply reads and prints all tokens
 * I included it as an example. You should compile the provided code
 * as it is and then run ./a.out < tests/test0.txt to see what this function
 * does
 */
void Parser::readAndPrintAllInput() {
  Token token_object;

  // get a token
  token_object = lexer.GetToken();

  // while end of input is not reached
  while (token_object.token_type != END_OF_FILE) {
    token_object.Print();             // pringt token
    token_object = lexer.GetToken();  // and get another one
  }

  // note that you should use END_OF_FILE and not EOF
}

/*
 * Initial function where the input is redirected towards. Once the input data
 * is read and evaluated, then END OF FILE is expected to close the execution of
 * the program.
 *
 * GRAMMAR:
 * input END_OF_FILE
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
void Parser::parseInput() {
  parse_input();
  expect(END_OF_FILE);

  // Once what type of error needs to be printed is checked, then we go ahead
  // and print that error here. If there are errors, we exit out of the program,
  // if not, we continue with lexical analysis of the input data.
  semantic_error();
}

/*
 * Function to parse the input tokens and the input data provided in the file.
 * The token details are processed by parse_tokens_section function and then the
 * input data is analyzed.
 *
 * GRAMMAR:
 * input -> inputs_section INPUT_TEXT
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
void Parser::parse_input() {
  parse_tokens_section();
  // TODO(kaustubh): myLexicalAnalysis to be added here.
  Token token_object = expect(INPUT_TEXT);
}

/*
 * Function to parse the inputs section containing multiple tokens.
 *
 * GRAMMAR:
 * tokens_section -> token_list HASH
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
void Parser::parse_tokens_section() {
  parse_token_list();
  expect(HASH);
}

/*
 * Function to parse the token list, which can contain one token or a list of
 * tokens.
 *
 * GRAMMAR:
 * token_list -> token
 * token_list -> token COMMA token_list
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
void Parser::parse_token_list() {
  parse_token();
  Token token_object = lexer.peek(1);
  if (token_object.token_type == COMMA) {
    expect(COMMA);
    parse_token_list();
  } else if (token_object.token_type == HASH) {
    return;
  } else {
    syntax_error();
  }
}

/*
 * Function for parsing individual token which has two parameters, ID and a
 * token_description. The token_id is used to check for the Expression Syntax
 * Error. If there are tokens with same token ids, this type of syntax error are
 * raised.
 *
 * GRAMMAR:
 * token -> ID expr
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
void Parser::parse_token() {
  Token token_id = expect(ID);
  int counter = 0;

  // Check for semantic error
  if (track_token.size() != 0) {
    for (int i = 0; i < static_cast<int>(track_token.size()); i++) {
      if (track_token[i].lexeme == token_id.lexeme) {
        counter = 1;
        std::string semantic_error_message;

        semantic_error_message = "Line " + std::to_string(token_id.line_no) +
                                 ": " + track_token[i].lexeme +
                                 " already declared on line " +
                                 std::to_string(track_token[i].line_no);

        track_semantic_error_messages.push_back(semantic_error_message);
      }
    }

    if (counter == 0) {
      track_token.push_back(token_id);
    }

  } else {
    track_token.push_back(token_id);
  }
  parse_expr(token_id);
}

/*
 * Function to parse the expression provided for the token description. This
 * function is different from the function that will be used to analyze the
 * input data.
 *
 * GRAMMAR:
 * expr -> CHAR
 * expr -> LPAREN expr RPAREN DOT LPAREN expr RPAREN
 * expr -> LPAREN expr RPAREN OR LPAREN expr RPAREN
 * expr -> LPAREN expr RPAREN STAR
 * expr -> UNDERSCORE
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
void Parser::parse_expr(Token token_id) {
  Token token_object_1 = lexer.peek(1);

  // If token type is Character, then we expect CHAR. After that, we can have
  // multiple input types like: LPAREN, RPAREN. If not it will throw a syntax
  // error
  if (token_object_1.token_type == CHAR) {
    expect_expr(CHAR, token_id);
  } else if (token_object_1.token_type == LPAREN) {
    // Parsing process for left paranthesis
    expect_expr(LPAREN, token_id);
    parse_expr(token_id);
    expect_expr(RPAREN, token_id);

    Token token_object_2 = lexer.peek(1);

    if (token_object_2.token_type == DOT) {
      expect_expr(DOT, token_id);
      expect_expr(LPAREN, token_id);
      parse_expr(token_id);
      expect_expr(RPAREN, token_id);
    } else if (token_object_2.token_type == OR) {
      expect_expr(OR, token_id);
      expect_expr(LPAREN, token_id);
      parse_expr(token_id);
      expect_expr(RPAREN, token_id);
    } else if (token_object_2.token_type == STAR) {
      expect_expr(STAR, token_id);
    } else {
      expression_syntax_error(token_id);
    }
  } else if (token_object_1.token_type == UNDERSCORE) {
    expect_expr(UNDERSCORE, token_id);
  } else {
    expression_syntax_error(token_id);
  }
}

/*
 *
 */
int main() {
  // note: the parser class has a lexer object instantiated in it (see file
  // parser.h). You should not be declaring a separate lexer object.
  // You can access the lexer object in the parser functions as shown in
  // the example  method Parser::readAndPrintAllInput()
  // If you declare another lexer object, lexical analysis will
  // not work correctly
  Parser parser;
  // parser.readAndPrintAllInput();
  parser.parseInput();
}
