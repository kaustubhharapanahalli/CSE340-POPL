// Copyright 2022 Kaustubh Harapanahalli
#ifndef TASKS_H_ // NOLINT
#define TASKS_H_

#include <algorithm>
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "execute.h"
#include "lexer.h" // NOLINT

struct stack_of_graph_nodes
{
    std::string statement_type;
    struct program_graph_node *expr = NULL;
    Token token_term;
};

struct program_graph_node
{
    std::string operation = "";
    struct program_graph_node *lhs = NULL;
    struct program_graph_node *rhs = NULL;
    Token token_term;
    std::string statement_type = "";
    int line_no = 0;
    int assign = 0;
};

class Parser
{
private:
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);

public:
    Parser();
    void parse_program();
    void parse_declaration_section();
    void parse_scalar_declaration_section();
    void parse_array_declaration_section();
    void parse_id_list(int id_type);

    void parse_block();
    void parse_statement_list();
    struct program_graph_node *parse_statement();
    struct program_graph_node *parse_assignment_statement();
    struct program_graph_node *parse_output_statement();
    struct program_graph_node *parse_variable_access(int check_assignment_state);
    Token check_end(int check_assignment_state);
    struct stack_of_graph_nodes *find_top_token(
        std::stack<struct stack_of_graph_nodes *> stk);
    struct program_graph_node *parse_expression(int check_assignment_state);

    void task1();
    void breadth_first_search(struct program_graph_node *root,
                              bool print_statement);

    void task2();
    std::string depth_first_search(struct program_graph_node *root);

    instNode *task3();
};

void parse_and_generate_AST();
void parse_and_type_check();
instNode *parse_and_generate_statement_list();

#endif // TASKS_H_  NOLINT
