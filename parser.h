/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef PARSER_H_
#define PARSER_H_

#include <set>
#include <string>
#include <vector>

#include "./lexer.h"

/*
 *Definition of RegularExpressionNode to generate the Non-definite Automata
 *Graph for the incoming input for each token
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
typedef struct RegularExpressionNode {
  RegularExpressionNode* first_neighbor;
  char first_label;
  RegularExpressionNode* second_neighbor;
  char second_label;
  int reg_id;

  RegularExpressionNode() {
    first_neighbor = NULL;
    first_label = '\0';
    second_neighbor = NULL;
    second_label = '\0';
  }
} RegularExpressionNode;

/*
 * Definition of RegularExpressionGraph to generate the Non-deterministic finite
 * Automata Graph for the incoming input for each token
 *
 * Written by: Kaustubh Manoj Harapanahalli
 */
typedef struct RegularExpressionGraph {
  RegularExpressionNode* start;
  RegularExpressionNode* accept;
} RegularExpressionGraph;

typedef struct tokenReg {
  std::string token_name;
  RegularExpressionGraph* reg;
} tokenReg;

static bool operator<(const RegularExpressionNode& n1,
                      const RegularExpressionNode& n2) {
  return n1.reg_id < n2.reg_id;
}

class myLexicalAnalyzer {
 public:
  void my_get_token();
  int match(RegularExpressionGraph* reg, std::string, int i);
  std::set<RegularExpressionNode> match_one_char(
      std::set<RegularExpressionNode> set1, char c);
  void set_input_string(const std::string& input_string);
  void set_tokens_list(const tokenReg& token);
  const std::vector<tokenReg>& get_tokens_list() const;
  std::vector<std::string> split(std::string str, char delimit);
  int get_counter() const;
  static void set_counter(int counter);
  bool check_node(std::set<RegularExpressionNode> S, RegularExpressionNode* r);
  bool compare_set(std::set<RegularExpressionNode> S1,
                   std::set<RegularExpressionNode> S2);
  void check_epsilon();

 private:
  std::vector<tokenReg> tokens_list;
  std::string input_string;
  static int counter;
};

class Parser {
 public:
  void parseInput();
  void parse_input();
  void readAndPrintAllInput();

 private:
  LexicalAnalyzer lexer;
  myLexicalAnalyzer my_lexer;
  void syntax_error();
  void expression_syntax_error(Token token_id);
  void semantic_error();
  Token expect_expr(TokenType expected_type, Token token_id);
  Token expect(TokenType expected_type);
  void parse_tokens_section();
  void parse_token_list();
  void parse_token();
  struct RegularExpressionGraph* parse_expr(Token token_id);
};

#endif  // PARSER_H_
