// Copyright 2022 Kaustubh Harapanahalli

#include "parser.h"  // NOLINT

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "execute.h"  // NOLINT
#include "lexer.h"    // NOLINT

std::map<std::string, int> var_location_table;

void Parser::syntax_error() {
  std::cout << "SNYATX EORRR !!!";
  exit(1);
}

Token Parser::expect(TokenType expected_type) {
  Token t = lexer.GetToken();
  if (t.token_type != expected_type) {
    syntax_error();
  }
  return t;
}

Parser::Parser() {}

struct InstructionNode* Parser::parse_program() {
  struct InstructionNode* program;
  Token token = lexer.peek(1);
  if (token.token_type == ID) {
    parse_variable_section();
  } else {
    syntax_error();
  }

  Token token2 = lexer.peek(1);
  if (token.token_type == ID) {
    program = parse_body();
  } else {
    syntax_error();
  }

  Token token3 = lexer.peek(1);
  if (token.token_type == ID) {
    parse_inputs();
  } else {
    syntax_error();
  }

  return program;
}

/*
 * Parsing of input variables:
 *
 * The input variables and their respective memory locations are stored as a
 * table - var_location_table, which contains the address of each of the
 * variable_int_map_node structure. This contains two values which are the index
 * number and the variable name associated with that index.
 */
void Parser::parse_variable_section() {
  Token t = lexer.peek(1);
  parse_id_list();
  expect(SEMICOLON);
}

void Parser::parse_id_list() {
  Token id_token = expect(ID);
  var_location_table[id_token.lexeme] = next_available;
  mem[next_available] = 0;
  next_available++;

  Token token;
  token = lexer.peek(1);

  if (token.token_type == COMMA) {
    expect(COMMA);
    parse_id_list();
  } else if (token.token_type == SEMICOLON) {
    return;
  }
}

/*
 * Reading the list of input arrays
 *
 * Reading the input arrays from the given program and storing all the values
 * in a vector.
 */
void Parser::parse_inputs() { parse_num_list(); }

void Parser::parse_num_list() {
  Token token = expect(NUM);
  inputs.push_back(std::stoi(token.lexeme));

  Token next = lexer.peek(1);

  if (next.token_type == NUM) {
    parse_num_list();
  } else if (next.token_type == END_OF_FILE) {
    return;
  }
}

/*
 * Parsing of the body
 *
 *
 */

struct InstructionNode* Parser::parse_body() {
  struct InstructionNode* program = nullptr;
  expect(LBRACE);
  program = parse_statement_list();
  expect(RBRACE);
  return program;
}

struct InstructionNode* Parser::parse_statement_list() {
  struct InstructionNode* instruction_i = nullptr;
  struct InstructionNode* instructions = nullptr;
  Token statement_token = lexer.peek(1);

  if (statement_token.token_type == RBRACE) {
    return instruction_i;
  }

  instruction_i = parse_statement();
  if (statement_token.token_type == ID | statement_token.token_type == WHILE |
      statement_token.token_type == IF | statement_token.token_type == SWITCH |
      statement_token.token_type == FOR | statement_token.token_type == OUTPUT |
      statement_token.token_type == INPUT) {
    instructions = parse_statement_list();
    struct InstructionNode* get_last_statement = instruction_i;
    while (get_last_statement->next != nullptr) {
      get_last_statement = get_last_statement->next;
    }
    get_last_statement->next = instructions;
  }
  return instruction_i;
}

struct InstructionNode* Parser::parse_statement() {
  struct InstructionNode* instruction = nullptr;
  struct InstructionNode* init = new InstructionNode;

  init->type = NOOP;
  init->next = nullptr;

  struct InstructionNode* get_last_instruction;

  Token statement_token = lexer.peek(1);

  switch (statement_token.token_type) {
    case ID:
      instruction = parse_assignment_statement();
      break;

    case WHILE:
      instruction = parse_while_statement();
      break;

    case IF:
      instruction = parse_if_statement();
      break;

    case SWITCH:
      instruction = parse_switch_statement(init);
      get_last_instruction = instruction;

      while (get_last_instruction->next != nullptr) {
        get_last_instruction = get_last_instruction->next;
      }

      get_last_instruction->next = init;
      break;

    case FOR:
      instruction = parse_for_statement();
      break;

    case OUTPUT:
      instruction = parse_output_statement();
      break;

    case INPUT:
      instruction = parse_input_statement();
      break;

    default:
      syntax_error();
  }

  return instruction;
}

struct InstructionNode* Parser::parse_input_statement() {
  struct InstructionNode* input_instruction = new InstructionNode;
  expect(INPUT);
  input_instruction->type = IN;
  Token t = expect(ID);
  input_instruction->input_inst.var_index = var_location_table[t.lexeme];
  input_instruction->next = nullptr;
  expect(SEMICOLON);
  return input_instruction;
}

struct InstructionNode* Parser::parse_output_statement() {
  struct InstructionNode* output_instruction = new InstructionNode;
  expect(OUTPUT);
  output_instruction->type = OUT;
  Token t = expect(ID);
  output_instruction->output_inst.var_index = var_location_table[t.lexeme];
  output_instruction->next = nullptr;
  expect(SEMICOLON);
  return output_instruction;
}

struct InstructionNode* Parser::parse_assignment_statement() {
  struct InstructionNode* assign_instruction = new InstructionNode;
  assign_instruction->type = ASSIGN;

  Token token = expect(ID);
  assign_instruction->assign_inst.left_hand_side_index =
      var_location_table[token.lexeme];

  expect(EQUAL);

  Token token_1 = lexer.peek(1);
  Token token_2 = lexer.peek(2);
  if (token_1.token_type == ID || token_1.token_type == NUM) {
    if (token_2.token_type == PLUS || token_2.token_type == MINUS ||
        token_2.token_type == MULT || token_2.token_type == DIV) {
      struct InstructionNode* expression_node = parse_expression();
      assign_instruction->assign_inst.opernd1_index =
          expression_node->assign_inst.opernd1_index;
      assign_instruction->assign_inst.op = expression_node->assign_inst.op;
      assign_instruction->assign_inst.opernd2_index =
          expression_node->assign_inst.opernd2_index;
    } else if (token_2.token_type == SEMICOLON) {
      assign_instruction->assign_inst.op = OPERATOR_NONE;
      assign_instruction->assign_inst.opernd1_index = parse_primary();
    } else {
      syntax_error();
    }
  } else {
    syntax_error();
  }
  expect(SEMICOLON);
  // set next to null
  assign_instruction->next = nullptr;
  return assign_instruction;
}

struct InstructionNode* Parser::parse_expression() {
  struct InstructionNode* get_operation_type = new InstructionNode;
  get_operation_type->assign_inst.opernd1_index = parse_primary();

  switch (parse_operation()) {
    case PLUS:
      get_operation_type->assign_inst.op = OPERATOR_PLUS;
      break;
    case MINUS:
      get_operation_type->assign_inst.op = OPERATOR_MINUS;
      break;
    case MULT:
      get_operation_type->assign_inst.op = OPERATOR_MULT;
      break;
    case DIV:
      get_operation_type->assign_inst.op = OPERATOR_DIV;
      break;
    default:
      break;
  }
  get_operation_type->assign_inst.opernd2_index = parse_primary();

  return get_operation_type;
}

int Parser::parse_primary() {
  int index = -1;
  Token token = lexer.GetToken();
  if (token.token_type == ID || token.token_type == NUM) {
    if (token.token_type == ID) {
      index = var_location_table[token.lexeme];
    } else {
      index = next_available;
      mem[next_available++] = stoi(token.lexeme);
    }
  } else {
    syntax_error();
  }
  return index;
}

TokenType Parser::parse_operation() {
  Token t = lexer.GetToken();
  if (t.token_type == PLUS || t.token_type == MINUS || t.token_type == MULT ||
      t.token_type == DIV) {
    return t.token_type;
  } else {
    syntax_error();
  }
}

struct InstructionNode* Parser::parse_if_statement() {
  struct InstructionNode* if_instruction = new InstructionNode;
  expect(IF);

  if_instruction->type = CJMP;
  struct InstructionNode* condition_check = parse_condition();
  if_instruction->cjmp_inst.opernd1_index =
      condition_check->cjmp_inst.opernd1_index;
  if_instruction->cjmp_inst.condition_op =
      condition_check->cjmp_inst.condition_op;
  if_instruction->cjmp_inst.opernd2_index =
      condition_check->cjmp_inst.opernd2_index;

  Token token = lexer.peek(1);
  if (token.token_type == LBRACE) {
    if_instruction->next = parse_body();
  } else {
    syntax_error();
  }

  struct InstructionNode* no_operation_node = new InstructionNode;
  no_operation_node->type = NOOP;
  no_operation_node->next = nullptr;

  struct InstructionNode* get_last_node = if_instruction;
  while (get_last_node->next != nullptr) {
    get_last_node = get_last_node->next;
  }
  get_last_node->next = no_operation_node;

  if_instruction->cjmp_inst.target = no_operation_node;

  return if_instruction;
}

struct InstructionNode* Parser::parse_condition() {
  struct InstructionNode* get_condition_node = new InstructionNode;

  Token token_1 = lexer.peek(1);
  if (token_1.token_type == ID || token_1.token_type == NUM) {
    get_condition_node->cjmp_inst.opernd1_index = parse_primary();
  } else {
    syntax_error();
  }

  Token token_2 = lexer.peek(1);
  if (token_2.token_type == GREATER || token_2.token_type == LESS ||
      token_2.token_type == NOTEQUAL) {
    switch (parse_relation_operation()) {
      case GREATER:
        get_condition_node->cjmp_inst.condition_op = CONDITION_GREATER;
        break;
      case LESS:
        get_condition_node->cjmp_inst.condition_op = CONDITION_LESS;
        break;
      case NOTEQUAL:
        get_condition_node->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
        break;
      default:
        break;
    }
  } else {
    syntax_error();
  }

  Token token_3 = lexer.peek(1);
  if (token_3.token_type == ID || token_3.token_type == NUM) {
    get_condition_node->cjmp_inst.opernd2_index = parse_primary();
  } else {
    syntax_error();
  }
  return get_condition_node;
}

TokenType Parser::parse_relation_operation() {
  Token token = lexer.GetToken();
  if (token.token_type == GREATER || token.token_type == LESS ||
      token.token_type == NOTEQUAL) {
    return token.token_type;
  } else {
    syntax_error();
  }
}

struct InstructionNode* Parser::parse_while_statement() {
  struct InstructionNode* while_instruction = new InstructionNode;
  expect(WHILE);
  while_instruction->type = CJMP;
  struct InstructionNode* nagisa = parse_condition();
  while_instruction->cjmp_inst.opernd1_index = nagisa->cjmp_inst.opernd1_index;
  while_instruction->cjmp_inst.condition_op = nagisa->cjmp_inst.condition_op;
  while_instruction->cjmp_inst.opernd2_index = nagisa->cjmp_inst.opernd2_index;

  Token token = lexer.peek(1);

  if (token.token_type == LBRACE) {
    while_instruction->next = parse_body();
  } else {
    syntax_error();
  }

  struct InstructionNode* jump_node = new InstructionNode;
  jump_node->type = JMP;
  jump_node->jmp_inst.target = while_instruction;

  struct InstructionNode* no_operation_node = new InstructionNode;
  no_operation_node->type = NOOP;
  no_operation_node->next = nullptr;

  struct InstructionNode* get_last_node = while_instruction;
  while (get_last_node->next != nullptr) {
    get_last_node = get_last_node->next;
  }
  get_last_node->next = jump_node;
  jump_node->next = no_operation_node;
  while_instruction->cjmp_inst.target = no_operation_node;

  return while_instruction;
}

struct InstructionNode* Parser::parse_switch_statement(
    struct InstructionNode* init) {
  struct InstructionNode* switch_instruction = new InstructionNode;
  expect(SWITCH);

  Token token_1 = expect(ID);
  int switch_operation = var_location_table[token_1.lexeme];
  expect(LBRACE);

  Token token_2 = lexer.peek(1);
  if (token_2.token_type == CASE) {
    switch_instruction = parse_case_list(switch_operation, init);

  } else {
    syntax_error();
  }

  Token token_3 = lexer.peek(1);
  if (token_3.token_type == DEFAULT) {
    struct InstructionNode* get_last_node = switch_instruction;
    while (get_last_node->next->next != nullptr) {
      get_last_node = get_last_node->next;
    }
    get_last_node->next = parse_default_case();

    expect(RBRACE);
  } else if (token_3.token_type == RBRACE) {
    token_3 = lexer.GetToken();
    return switch_instruction;
  } else {
    syntax_error();
  }
  return switch_instruction;
}

struct InstructionNode* Parser::parse_case_list(int operation,
                                                struct InstructionNode* init) {
  struct InstructionNode* case_node = new InstructionNode;
  struct InstructionNode* case_list_node = nullptr;

  Token token_1 = lexer.peek(1);
  if (token_1.token_type == CASE) {
    case_node = parse_case(operation);

    struct InstructionNode* jump_node = new InstructionNode;
    jump_node->type = JMP;
    jump_node->jmp_inst.target = init;
    struct InstructionNode* get_last_node = case_node->cjmp_inst.target;
    while (get_last_node->next->next != nullptr) {
      get_last_node = get_last_node->next;
    }
    get_last_node->next = jump_node;

  } else {
    syntax_error();
  }

  Token token_2 = lexer.peek(1);
  if (token_2.token_type == CASE) {
    case_list_node = parse_case_list(operation, init);

    struct InstructionNode* get_last_node = case_node;
    while (get_last_node->next->next != nullptr) {
      get_last_node = get_last_node->next;
    }
    get_last_node->next = case_list_node;
  }
  return case_node;
}

struct InstructionNode* Parser::parse_case(int op) {
  struct InstructionNode* case_instruction = new InstructionNode;
  expect(CASE);
  case_instruction->type = CJMP;
  case_instruction->cjmp_inst.opernd1_index = op;
  case_instruction->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
  Token token_1 = expect(NUM);

  int index = next_available;
  mem[next_available++] = std::stoi(token_1.lexeme);
  case_instruction->cjmp_inst.opernd2_index = index;

  expect(COLON);
  Token token_2 = lexer.peek(1);
  if (token_2.token_type == LBRACE) {
    case_instruction->cjmp_inst.target = parse_body();
  } else {
    syntax_error();
  }

  struct InstructionNode* no_operation = new InstructionNode;
  no_operation->type = NOOP;
  no_operation->next = nullptr;

  struct InstructionNode* get_last_node = case_instruction->cjmp_inst.target;
  while (get_last_node->next != nullptr) {
    get_last_node = get_last_node->next;
  }
  case_instruction->next = no_operation;
  get_last_node->next = case_instruction->next;
  return case_instruction;
}

struct InstructionNode* Parser::parse_default_case() {
  struct InstructionNode* deafault_instruction = new InstructionNode;
  expect(DEFAULT);
  expect(COLON);

  Token token = lexer.peek(1);
  if (token.token_type == LBRACE) {
    deafault_instruction = parse_body();
  } else {
    syntax_error();
  }
  return deafault_instruction;
}

struct InstructionNode* Parser::parse_for_statement() {
  struct InstructionNode* for_instruction = new InstructionNode;
  struct InstructionNode* assignStmt = new InstructionNode;
  expect(FOR);
  expect(LPAREN);

  // assign_stmt
  Token token_1 = lexer.peek(1);
  if (token_1.token_type == ID) {
    for_instruction = parse_assignment_statement();
  } else {
    syntax_error();
  }

  auto* condition_instruction = new InstructionNode;
  condition_instruction->type = CJMP;
  // condition
  struct InstructionNode* condition_node = parse_condition();
  condition_instruction->cjmp_inst.opernd1_index =
      condition_node->cjmp_inst.opernd1_index;
  condition_instruction->cjmp_inst.condition_op =
      condition_node->cjmp_inst.condition_op;
  condition_instruction->cjmp_inst.opernd2_index =
      condition_node->cjmp_inst.opernd2_index;

  expect(SEMICOLON);

  Token token_2 = lexer.peek(1);
  if (token_2.token_type == ID) {
    assignStmt = parse_assignment_statement();
    assignStmt->next = nullptr;
    expect(RPAREN);
  } else {
    syntax_error();
  }

  Token token_3 = lexer.peek(1);
  if (token_3.token_type == LBRACE) {
    condition_instruction->next = parse_body();
  } else {
    syntax_error();
  }

  struct InstructionNode* increment_statement = condition_instruction->next;
  while (increment_statement->next != nullptr) {
    increment_statement = increment_statement->next;
  }
  increment_statement->next = assignStmt;

  struct InstructionNode* jump_node = new InstructionNode;
  jump_node->type = JMP;
  jump_node->jmp_inst.target = condition_instruction;

  struct InstructionNode* no_operation = new InstructionNode;
  no_operation->type = NOOP;
  no_operation->next = nullptr;

  jump_node->next = no_operation;

  struct InstructionNode* getLast = condition_instruction;
  while (getLast->next != nullptr) {
    getLast = getLast->next;
  }
  getLast->next = jump_node;
  condition_instruction->cjmp_inst.target = no_operation;

  for_instruction->next = condition_instruction;

  return for_instruction;
}
