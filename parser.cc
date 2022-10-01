/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 * =============================================================================
 * ################################# COMMENTS #################################
 * =============================================================================
 *
 * The complete implementation of the functions for parsing the input grammar,
 * checking for syntax, semantic and epsilon errors and lexical analysis are all
 * executed in the parser script itself. The other two scripts (lexer.cc and
 * inputbuf.cc) are untouched. Hence I have not added any comments to these two
 * files.
 */
#include "./parser.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <istream>
#include <set>
#include <sstream>
#include <vector>

// Definition of vector to track token values to assist in raising expression
// syntax error when required.
std::vector<Token> track_token;
std::vector<std::string> track_semantic_error_messages;

// The below set of code is to read the input provided in the Token section

/*
 * Function provides a simplified way of throwing general syntax error.
 */
void Parser::syntax_error() {
  std::cout << "SNYTAX ERORR" << std::endl;
  exit(1);
}

/*
 * Function provides a simplified way of throwing expression syntax error.
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
void Parser::expression_syntax_error(Token token_id) {
  std::cout << "SYNTAX ERROR IN EXPRESSION OF " + token_id.lexeme << std::endl;
  exit(1);
}

/*
 * Function provides a simplified way of throwing semantic error.
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
void Parser::semantic_error() {
  for (int i = 0; i < static_cast<int>(track_semantic_error_messages.size());
       i++) {
    std::cout << track_semantic_error_messages[i] << std::endl;
  }
  exit(1);
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

/*
 * Function to check if the definition of the token id is defined according to
 * the rules or not.
 *
 * GRAMMAR:
 * ID -> LETTER . CHAR*
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
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
  if (!track_semantic_error_messages.empty()) {
    semantic_error();
  }
  my_lexer.my_get_token();
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

  std::string temp =
      token_object.lexeme.substr(1, token_object.lexeme.size() - 2);
  if (temp.at(0) == ' ') {
    temp = temp.substr(1, temp.size());
  }
  if (temp.at(temp.size() - 1) == ' ') {
    temp = temp.substr(0, temp.size() - 1);
  }
  //    cout << "Input string :" << temp << endl;
  my_lexer.set_input_string(temp);
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

  RegularExpressionGraph *reg = parse_expr(token_id);
  tokenReg tok;
  tok.token_name = token_id.lexeme;
  tok.reg = reg;
  my_lexer.set_tokens_list(tok);
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
struct RegularExpressionGraph *Parser::parse_expr(Token token_id) {
  Token token_object_1 = lexer.peek(1);

  if (token_object_1.token_type == CHAR) {
    Token t = expect_expr(CHAR, token_id);

    RegularExpressionGraph *reg = new RegularExpressionGraph();
    RegularExpressionNode *reg_node_1 = new RegularExpressionNode();

    reg_node_1->reg_id = my_lexer.get_counter();
    reg_node_1->first_label = t.lexeme.at(0);
    RegularExpressionNode *node2 = new RegularExpressionNode();
    node2->reg_id = my_lexer.get_counter();
    reg_node_1->first_neighbor = node2;
    reg->start = reg_node_1;
    reg->accept = node2;
    return reg;

  } else if (token_object_1.token_type == LPAREN) {
    expect_expr(LPAREN, token_id);
    RegularExpressionGraph *reg1 = parse_expr(token_id);
    expect_expr(RPAREN, token_id);

    Token token_object_2 = lexer.peek(1);

    if (token_object_2.token_type == DOT) {
      expect_expr(DOT, token_id);
      expect_expr(LPAREN, token_id);
      RegularExpressionGraph *reg2 = parse_expr(token_id);
      expect_expr(RPAREN, token_id);
      reg1->accept->first_neighbor = reg2->start;
      reg1->accept->first_label = '_';
      reg1->accept = reg2->accept;
      return reg1;

    } else if (token_object_2.token_type == OR) {
      expect_expr(OR, token_id);
      expect_expr(LPAREN, token_id);
      RegularExpressionGraph *reg2 = parse_expr(token_id);
      expect_expr(RPAREN, token_id);
      RegularExpressionNode *start = new RegularExpressionNode();
      start->reg_id = my_lexer.get_counter();
      start->first_label = '_';
      start->second_label = '_';
      start->first_neighbor = reg1->start;
      start->second_neighbor = reg2->start;
      RegularExpressionNode *accept = new RegularExpressionNode();
      accept->reg_id = my_lexer.get_counter();
      reg1->accept->first_label = '_';
      reg1->accept->first_neighbor = accept;
      reg2->accept->first_label = '_';
      reg2->accept->first_neighbor = accept;
      reg1->start = start;
      reg1->accept = accept;
      return reg1;

    } else if (token_object_2.token_type == STAR) {
      expect_expr(STAR, token_id);
      RegularExpressionNode *start = new RegularExpressionNode();
      start->reg_id = my_lexer.get_counter();
      start->first_label = '_';
      start->second_label = '_';
      start->first_neighbor = reg1->start;
      RegularExpressionNode *accept = new RegularExpressionNode();
      accept->reg_id = my_lexer.get_counter();
      start->second_neighbor = accept;
      reg1->accept->first_label = '_';
      reg1->accept->first_neighbor = accept;
      reg1->accept->second_label = '_';
      reg1->accept->second_neighbor = reg1->start;
      reg1->start = start;
      reg1->accept = accept;
      return reg1;

    } else {
      expression_syntax_error(token_id);
    }

  } else if (token_object_1.token_type == UNDERSCORE) {
    expect_expr(UNDERSCORE, token_id);
    RegularExpressionGraph *reg = new RegularExpressionGraph();
    RegularExpressionNode *node1 = new RegularExpressionNode();
    node1->reg_id = my_lexer.get_counter();
    node1->first_label = '_';
    RegularExpressionNode *node2 = new RegularExpressionNode();
    node2->reg_id = my_lexer.get_counter();
    node1->first_neighbor = node2;
    reg->start = node1;
    reg->accept = node2;
    return reg;

  } else {
    expression_syntax_error(token_id);
  }

  return NULL;
}

/*
 * Function definition for matching single characters between the generated
 * graph and the input string. Generates a set of Regular Expression Graph IDs
 * based on the input character.
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
std::set<RegularExpressionNode> myLexicalAnalyzer::match_one_char(
    std::set<RegularExpressionNode> S, char c) {
  std::set<RegularExpressionNode> S1;
  std::set<RegularExpressionNode>::iterator it;
  for (it = S.begin(); it != S.end(); ++it) {
    if (it->first_label == c && !check_node(S1, it->first_neighbor)) {
      S1.insert(*it->first_neighbor);
    }

    if (it->second_label == c && !check_node(S1, it->second_neighbor)) {
      S1.insert(*it->second_neighbor);
    }
  }

  if (S1.empty()) {
    return S1;
  }

  bool changed = true;
  std::set<RegularExpressionNode> S2;

  while (changed) {
    changed = false;

    for (it = S1.begin(); it != S1.end(); ++it) {
      S2.insert(*it);
      std::set<RegularExpressionNode>::iterator ik;

      if (it->first_label == '_' && !check_node(S2, it->first_neighbor)) {
        S2.insert(*it->first_neighbor);
        std::set<RegularExpressionNode>::iterator ik;
      }

      if (it->second_label == '_' && !check_node(S2, it->second_neighbor)) {
        S2.insert(*it->second_neighbor);
      }
    }

    if (!compare_set(S1, S2)) {
      changed = true;
      S1.insert(S2.begin(), S2.end());
      S2.clear();
    }
  }

  return S1;
}

/*
 * Function definition for matching characters between the generated graph and
 * the input string. Generates a set of Regular Expression Graph IDs based on
 * the input character.
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
int myLexicalAnalyzer::match(RegularExpressionGraph *reg, std::string input,
                             int position) {
  std::set<RegularExpressionNode> set_2;
  set_2.insert(*reg->start);
  std::set<RegularExpressionNode>::iterator it;
  std::set<RegularExpressionNode> set_1;
  set_1 = match_one_char(set_2, '_');
  set_1.insert(set_2.begin(), set_2.end());
  set_2.insert(set_1.begin(), set_1.end());

  int size = 0;
  int m = 0;
  for (std::string::iterator it = input.begin() + position, end = input.end();
       it != end; ++it) {
    size++;
    set_2 = match_one_char(set_2, *it);
    std::set<RegularExpressionNode>::iterator ik;

    if (set_2.find(*reg->accept) != set_2.end()) {
      m = size;
    }
  }

  return m;
}

/*
 * Lexical analyzer for if an input type of epsilon exists. If epsilon pipeline
 * exists, then throw an epsilon is not a token error and exit the program.
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
void myLexicalAnalyzer::check_epsilon() {
  int start = 0;
  std::vector<tokenReg>::iterator it;
  std::string lex = "";
  std::vector<std::string> splits = split("_", ' ');

  for (int i = 0; i < splits.size(); i++) {
    start = 0;

    while (start != splits[i].size()) {
      std::vector<tokenReg> tokens = get_tokens_list();

      for (it = tokens.begin(); it != tokens.end(); ++it) {
        tokenReg &reg = *it;
        int lex_size = match(reg.reg, splits[i], start);

        if (lex_size > 0) {
          lex.append(" ");
          lex.append(reg.token_name);
        }
      }

      if (!lex.empty()) {
        std::cout << "EPSILON IS NOOOOOOOT A TOKEN !!!" << lex << std::endl;
        exit(1);
      }

      if (lex.empty()) {
        return;
      }
    }
  }
}

/*
 * Lexical analyzer for providing the output for the properly defined input
 * strings. Based on the number of tokens present, the type of output message is
 * decided.
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
void myLexicalAnalyzer::my_get_token() {
  int start = 0;
  std::vector<tokenReg>::iterator it;
  std::string lex = "";
  std::vector<std::string> splits = split(input_string, ' ');

  check_epsilon();

  for (int i = 0; i < splits.size(); i++) {
    start = 0;

    while (start != splits[i].size()) {
      int max = 0;
      std::vector<tokenReg> tokens = get_tokens_list();

      for (it = tokens.begin(); it != tokens.end(); ++it) {
        tokenReg &reg = *it;
        int lex_size = match(reg.reg, splits[i], start);

        if (lex_size > max) {
          max = lex_size;
          lex = reg.token_name;
        }
      }

      if (max == 0) {
        std::cout << "ERROR";
        return;
      }

      std::cout << lex << " , \"" << splits[i].substr(start, max) << "\""
                << std::endl;
      start += max;
    }
  }
}

/*
 * Function to relate the input string provided in the input data to the
 * function - myLexicalAnalyzer for executing lexical analysis after syntax and
 * semantic error checks.
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
void myLexicalAnalyzer::set_input_string(const std::string &input_string) {
  myLexicalAnalyzer::input_string = input_string;
}

/*
 * Simplified function to append list of token registries for tracking the token
 * description for a given token id.
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
void myLexicalAnalyzer::set_tokens_list(const tokenReg &token) {
  myLexicalAnalyzer::tokens_list.push_back(token);
}

/*
 * Simplified function for returning the list of available tokens.
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
const std::vector<tokenReg> &myLexicalAnalyzer::get_tokens_list() const {
  return tokens_list;
}

/*
 * Function to split the input from the user for managing output generation
 * based on the number of input strings available.
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
std::vector<std::string> myLexicalAnalyzer::split(std::string s, char delimit) {
  std::vector<std::string> temp;
  std::stringstream ss(s);  // Turn the string into a stream.
  std::string t;

  while (getline(ss, t, delimit)) {
    temp.push_back(t);
  }

  return temp;
}

int myLexicalAnalyzer::counter;

/*
 * Counter to track the number of Regular expression nodes that are present in
 * the selected regular expression graph for easier assignment and tracking of
 * regular expression graph node tracking.
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
int myLexicalAnalyzer::get_counter() const {
  counter++;
  return counter;
}

/*
 * Simplified function to set the counter value to a variable related to the
 * class.
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
void myLexicalAnalyzer::set_counter(int count) { counter = count; }

/*
 * Function to compare two input nodes, one obtained from set of of tokens and
 * the other from the token description input.
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
bool myLexicalAnalyzer::check_node(std::set<RegularExpressionNode> S,
                                   RegularExpressionNode *r) {
  std::set<RegularExpressionNode>::iterator i;
  for (i = S.begin(); i != S.end(); ++i) {
    if (r->reg_id == i->reg_id) {
      return true;
    }
  }
  return false;
}

/*
 * Function to compare if two input sets have the same values.
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
bool myLexicalAnalyzer::compare_set(std::set<RegularExpressionNode> S1,
                                    std::set<RegularExpressionNode> S2) {
  if (S1.size() != S2.size()) {
    return false;
  }

  std::set<RegularExpressionNode>::iterator i;
  int s1[S1.size()];
  int s2[S2.size()];
  int j = 0, k = 0;

  for (i = S1.begin(); i != S1.end(); ++i) {
    s1[j] = i->reg_id;
    j++;
  }
  std::set<RegularExpressionNode>::iterator t;

  for (i = S2.begin(); i != S2.end(); ++i) {
    s2[k] = i->reg_id;
    k++;
  }

  std::sort(s1, s1 + j);
  std::sort(s2, s2 + k);

  for (int m = 0; m < k; m++) {
    if (s1[m] != s2[m]) {
      return false;
    }
  }

  return true;
}

/*
 *
 */
int main() {
  Parser parser;
  parser.parseInput();
}
