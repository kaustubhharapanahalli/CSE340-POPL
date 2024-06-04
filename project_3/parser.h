// Copyright 2022 Kaustubh Harapanahalli

#ifndef PARSER_H_
#define PARSER_H_

#include <algorithm>
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "execute.h"  // NOLINT
#include "lexer.h"    // NOLINT

struct variable_int_map_node {
  std::string var_name;
  int mem_index;
  int var_value;
};

class Parser {
 private:
  LexicalAnalyzer lexer;
  void syntax_error();
  Token expect(TokenType expected_type);

 public:
  Parser();
  struct InstructionNode* parse_program();
  void parse_variable_section();
  void parse_id_list();
  struct variable_int_map_node* return_location(Token var_token);
  // void return_location(Token var_token);

  void parse_inputs();

  struct InstructionNode* parse_body();
  struct InstructionNode* parse_statement_list();
  struct InstructionNode* parse_statement();

  struct InstructionNode* parse_assignment_statement();
  struct InstructionNode* parse_while_statement();
  struct InstructionNode* parse_if_statement();
  struct InstructionNode* parse_switch_statement(struct InstructionNode* init);
  struct InstructionNode* parse_for_statement();
  struct InstructionNode* parse_output_statement();
  struct InstructionNode* parse_input_statement();
  TokenType parse_operation();
  TokenType parse_relation_operation();

  int parse_primary();
  struct InstructionNode* parse_expression();

  struct InstructionNode* parse_condition();

  struct InstructionNode* parse_case_list(int switch_opertion,
                                          struct InstructionNode* init);
  struct InstructionNode* parse_case(int operation);
  struct InstructionNode* parse_default_case();

  void parse_num_list();
};

#endif  // PARSER_H_
