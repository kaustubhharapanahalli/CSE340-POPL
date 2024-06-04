// Copyright 2022 Kaustubh Harapanahalli
#include "tasks.h" // NOLINT

#include <algorithm>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "execute.h" // NOLINT

extern long mem[MEMSIZE]; // NOLINT

std::unordered_map<std::string, std::unordered_map<std::string, int>>
    operator_precedence_table;
std::unordered_map<std::string, std::pair<std::string, std::vector<int>>>
    grammar_expression_to_string_table;
std::unordered_set<std::string> valid_code_grammar_strings;
std::vector<struct program_graph_node *> program_statement_table;
std::unordered_set<std::string> scalar_variables;
std::unordered_set<std::string> array_variables;
std::unordered_set<std::string> valid_statement_grammar_strings;
std::vector<int> typelino;
std::vector<int> assignlino;

int memory_position = 0;
std::unordered_map<std::string, int> variable_location_in_memory;
std::vector<std::string> all_grammar_keys_for_comparison = {
    "END_OF_FILE", "SCALAR", "ARRAY", "OUTPUT", "PLUS", "MINUS", "DIV",
    "MULT", "EQUAL", "SEMICOLON", "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "LBRACE", "RBRACE", "DOT", "NUM", "ID", "ERROR"};

void Parser::syntax_error()
{
    std::cout << "SNYATX EORRR !!!";
    exit(1);
}

Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
    {
        syntax_error();
    }
    return t;
}

Parser::Parser()
{
    grammar_expression_to_string_table["ID"] = make_pair("", std::vector<int>{0});
    grammar_expression_to_string_table["NUM"] =
        make_pair("", std::vector<int>{0});
    grammar_expression_to_string_table["EXPRLBRACDOTRBRAC"] =
        make_pair("[.]", std::vector<int>{0});
    grammar_expression_to_string_table["EXPRLBRACEXPRRBRAC"] =
        make_pair("[]", std::vector<int>{0, 2});
    grammar_expression_to_string_table["EXPRPLUSEXPR"] =
        make_pair("+", std::vector<int>{0, 2});
    grammar_expression_to_string_table["EXPRMINUSEXPR"] =
        make_pair("-", std::vector<int>{0, 2});
    grammar_expression_to_string_table["EXPRDIVEXPR"] =
        make_pair("/", std::vector<int>{0, 2});
    grammar_expression_to_string_table["EXPRMULTEXPR"] =
        make_pair("*", std::vector<int>{0, 2});
    grammar_expression_to_string_table["LPARENEXPRRPAREN"] =
        make_pair("", std::vector<int>{1});

    valid_code_grammar_strings.insert("ID");
    valid_code_grammar_strings.insert("NUM");
    valid_code_grammar_strings.insert("PLUS");
    valid_code_grammar_strings.insert("MINUS");
    valid_code_grammar_strings.insert("MULT");
    valid_code_grammar_strings.insert("DIV");
    valid_code_grammar_strings.insert("LPAREN");
    valid_code_grammar_strings.insert("RPAREN");
    valid_code_grammar_strings.insert("DOT");
    valid_code_grammar_strings.insert("RBRAC");
    valid_code_grammar_strings.insert("LBRAC");

    valid_statement_grammar_strings.insert("ID");
    valid_statement_grammar_strings.insert("NUM");
    valid_statement_grammar_strings.insert("EXPRLBRACDOTRBRAC");
    valid_statement_grammar_strings.insert("EXPRLBRACEXPRRBRAC");
    valid_statement_grammar_strings.insert("EXPRPLUSEXPR");
    valid_statement_grammar_strings.insert("EXPRMINUSEXPR");
    valid_statement_grammar_strings.insert("EXPRDIVEXPR");
    valid_statement_grammar_strings.insert("EXPRMULTEXPR");
    valid_statement_grammar_strings.insert("LPARENEXPRRPAREN");

    operator_precedence_table["PLUS"]["PLUS"] = 0;
    operator_precedence_table["PLUS"]["MINUS"] = 0;
    operator_precedence_table["PLUS"]["MULT"] = 1;
    operator_precedence_table["PLUS"]["DIV"] = 1;
    operator_precedence_table["PLUS"]["LPAREN"] = 1;
    operator_precedence_table["PLUS"]["RPAREN"] = 0;
    operator_precedence_table["PLUS"]["LBRAC"] = 1;
    operator_precedence_table["PLUS"]["DOT"] = 3;
    operator_precedence_table["PLUS"]["RBRAC"] = 0;
    operator_precedence_table["PLUS"]["NUM"] = 1;
    operator_precedence_table["PLUS"]["ID"] = 1;
    operator_precedence_table["PLUS"]["END_OF_FILE"] = 0;

    operator_precedence_table["MINUS"]["PLUS"] = 0;
    operator_precedence_table["MINUS"]["MINUS"] = 0;
    operator_precedence_table["MINUS"]["MULT"] = 1;
    operator_precedence_table["MINUS"]["DIV"] = 1;
    operator_precedence_table["MINUS"]["LPAREN"] = 1;
    operator_precedence_table["MINUS"]["RPAREN"] = 0;
    operator_precedence_table["MINUS"]["LBRAC"] = 1;
    operator_precedence_table["MINUS"]["DOT"] = 3;
    operator_precedence_table["MINUS"]["RBRAC"] = 0;
    operator_precedence_table["MINUS"]["NUM"] = 1;
    operator_precedence_table["MINUS"]["ID"] = 1;
    operator_precedence_table["MINUS"]["END_OF_FILE"] = 0;

    operator_precedence_table["MULT"]["PLUS"] = 0;
    operator_precedence_table["MULT"]["MINUS"] = 0;
    operator_precedence_table["MULT"]["MULT"] = 0;
    operator_precedence_table["MULT"]["DIV"] = 0;
    operator_precedence_table["MULT"]["LPAREN"] = 1;
    operator_precedence_table["MULT"]["RPAREN"] = 0;
    operator_precedence_table["MULT"]["LBRAC"] = 1;
    operator_precedence_table["MULT"]["DOT"] = 3;
    operator_precedence_table["MULT"]["RBRAC"] = 0;
    operator_precedence_table["MULT"]["NUM"] = 1;
    operator_precedence_table["MULT"]["ID"] = 1;
    operator_precedence_table["MULT"]["END_OF_FILE"] = 0;

    operator_precedence_table["DIV"]["PLUS"] = 0;
    operator_precedence_table["DIV"]["MINUS"] = 0;
    operator_precedence_table["DIV"]["MULT"] = 0;
    operator_precedence_table["DIV"]["DIV"] = 0;
    operator_precedence_table["DIV"]["LPAREN"] = 1;
    operator_precedence_table["DIV"]["RPAREN"] = 0;
    operator_precedence_table["DIV"]["LBRAC"] = 1;
    operator_precedence_table["DIV"]["DOT"] = 3;
    operator_precedence_table["DIV"]["RBRAC"] = 0;
    operator_precedence_table["DIV"]["NUM"] = 1;
    operator_precedence_table["DIV"]["ID"] = 1;
    operator_precedence_table["DIV"]["END_OF_FILE"] = 0;

    operator_precedence_table["LPAREN"]["PLUS"] = 1;
    operator_precedence_table["LPAREN"]["MINUS"] = 1;
    operator_precedence_table["LPAREN"]["MULT"] = 1;
    operator_precedence_table["LPAREN"]["DIV"] = 1;
    operator_precedence_table["LPAREN"]["LPAREN"] = 1;
    operator_precedence_table["LPAREN"]["RPAREN"] = 2;
    operator_precedence_table["LPAREN"]["LBRAC"] = 1;
    operator_precedence_table["LPAREN"]["DOT"] = 3;
    operator_precedence_table["LPAREN"]["RBRAC"] = 1;
    operator_precedence_table["LPAREN"]["NUM"] = 1;
    operator_precedence_table["LPAREN"]["ID"] = 1;
    operator_precedence_table["LPAREN"]["END_OF_FILE"] = 3;

    operator_precedence_table["RPAREN"]["PLUS"] = 0;
    operator_precedence_table["RPAREN"]["MINUS"] = 0;
    operator_precedence_table["RPAREN"]["MULT"] = 0;
    operator_precedence_table["RPAREN"]["DIV"] = 0;
    operator_precedence_table["RPAREN"]["LPAREN"] = 3;
    operator_precedence_table["RPAREN"]["RPAREN"] = 0;
    operator_precedence_table["RPAREN"]["LBRAC"] = 0;
    operator_precedence_table["RPAREN"]["DOT"] = 3;
    operator_precedence_table["RPAREN"]["RBRAC"] = 0;
    operator_precedence_table["RPAREN"]["NUM"] = 3;
    operator_precedence_table["RPAREN"]["ID"] = 3;
    operator_precedence_table["RPAREN"]["END_OF_FILE"] = 0;

    operator_precedence_table["LBRAC"]["PLUS"] = 1;
    operator_precedence_table["LBRAC"]["MINUS"] = 1;
    operator_precedence_table["LBRAC"]["MULT"] = 1;
    operator_precedence_table["LBRAC"]["DIV"] = 1;
    operator_precedence_table["LBRAC"]["LPAREN"] = 1;
    operator_precedence_table["LBRAC"]["RPAREN"] = 1;
    operator_precedence_table["LBRAC"]["LBRAC"] = 1;
    operator_precedence_table["LBRAC"]["DOT"] = 2;
    operator_precedence_table["LBRAC"]["RBRAC"] = 2;
    operator_precedence_table["LBRAC"]["NUM"] = 1;
    operator_precedence_table["LBRAC"]["ID"] = 1;
    operator_precedence_table["LBRAC"]["END_OF_FILE"] = 3;

    operator_precedence_table["DOT"]["PLUS"] = 3;
    operator_precedence_table["DOT"]["MINUS"] = 3;
    operator_precedence_table["DOT"]["MULT"] = 3;
    operator_precedence_table["DOT"]["DIV"] = 3;
    operator_precedence_table["DOT"]["LPAREN"] = 3;
    operator_precedence_table["DOT"]["RPAREN"] = 3;
    operator_precedence_table["DOT"]["LBRAC"] = 3;
    operator_precedence_table["DOT"]["DOT"] = 3;
    operator_precedence_table["DOT"]["RBRAC"] = 2;
    operator_precedence_table["DOT"]["NUM"] = 3;
    operator_precedence_table["DOT"]["ID"] = 3;
    operator_precedence_table["DOT"]["END_OF_FILE"] = 3;

    operator_precedence_table["RBRAC"]["PLUS"] = 0;
    operator_precedence_table["RBRAC"]["MINUS"] = 0;
    operator_precedence_table["RBRAC"]["MULT"] = 0;
    operator_precedence_table["RBRAC"]["DIV"] = 0;
    operator_precedence_table["RBRAC"]["LPAREN"] = 3;
    operator_precedence_table["RBRAC"]["RPAREN"] = 0;
    operator_precedence_table["RBRAC"]["LBRAC"] = 0;
    operator_precedence_table["RBRAC"]["DOT"] = 3;
    operator_precedence_table["RBRAC"]["RBRAC"] = 0;
    operator_precedence_table["RBRAC"]["NUM"] = 3;
    operator_precedence_table["RBRAC"]["ID"] = 3;
    operator_precedence_table["RBRAC"]["END_OF_FILE"] = 0;

    operator_precedence_table["NUM"]["PLUS"] = 0;
    operator_precedence_table["NUM"]["MINUS"] = 0;
    operator_precedence_table["NUM"]["MULT"] = 0;
    operator_precedence_table["NUM"]["DIV"] = 0;
    operator_precedence_table["NUM"]["LPAREN"] = 3;
    operator_precedence_table["NUM"]["RPAREN"] = 0;
    operator_precedence_table["NUM"]["LBRAC"] = 0;
    operator_precedence_table["NUM"]["DOT"] = 3;
    operator_precedence_table["NUM"]["RBRAC"] = 0;
    operator_precedence_table["NUM"]["NUM"] = 3;
    operator_precedence_table["NUM"]["ID"] = 3;
    operator_precedence_table["NUM"]["END_OF_FILE"] = 0;

    operator_precedence_table["ID"]["PLUS"] = 0;
    operator_precedence_table["ID"]["MINUS"] = 0;
    operator_precedence_table["ID"]["MULT"] = 0;
    operator_precedence_table["ID"]["DIV"] = 0;
    operator_precedence_table["ID"]["LPAREN"] = 3;
    operator_precedence_table["ID"]["RPAREN"] = 0;
    operator_precedence_table["ID"]["LBRAC"] = 0;
    operator_precedence_table["ID"]["DOT"] = 3;
    operator_precedence_table["ID"]["RBRAC"] = 0;
    operator_precedence_table["ID"]["NUM"] = 3;
    operator_precedence_table["ID"]["ID"] = 3;
    operator_precedence_table["ID"]["END_OF_FILE"] = 0;

    operator_precedence_table["END_OF_FILE"]["PLUS"] = 1;
    operator_precedence_table["END_OF_FILE"]["MINUS"] = 1;
    operator_precedence_table["END_OF_FILE"]["MULT"] = 1;
    operator_precedence_table["END_OF_FILE"]["DIV"] = 1;
    operator_precedence_table["END_OF_FILE"]["LPAREN"] = 1;
    operator_precedence_table["END_OF_FILE"]["RPAREN"] = 3;
    operator_precedence_table["END_OF_FILE"]["LBRAC"] = 1;
    operator_precedence_table["END_OF_FILE"]["DOT"] = 3;
    operator_precedence_table["END_OF_FILE"]["RBRAC"] = 3;
    operator_precedence_table["END_OF_FILE"]["NUM"] = 1;
    operator_precedence_table["END_OF_FILE"]["ID"] = 1;
    operator_precedence_table["END_OF_FILE"]["END_OF_FILE"] = 4;
}

void Parser::parse_program()
{
    parse_declaration_section();
    parse_block();
    expect(END_OF_FILE);
}

void Parser::parse_declaration_section()
{
    parse_scalar_declaration_section();
    parse_array_declaration_section();
}

void Parser::parse_array_declaration_section()
{
    expect(ARRAY);
    parse_id_list(0);
}

void Parser::parse_scalar_declaration_section()
{
    expect(SCALAR);
    parse_id_list(1);
}

void Parser::parse_id_list(int id_type)
{
    Token token1 = expect(ID);

    if (id_type == 0)
    {
        variable_location_in_memory[token1.lexeme] = memory_position;
        int temp = 0;

        while (temp < 10)
        {
            mem[memory_position + temp] = 0;
            temp++;
        }
        memory_position += 10;
        array_variables.insert(token1.lexeme);
    }
    else
    {
        variable_location_in_memory[token1.lexeme] = memory_position;
        mem[memory_position] = 0;
        memory_position++;
        scalar_variables.insert(token1.lexeme);
    }

    Token token2;
    token2 = lexer.peek(1);
    if (token2.token_type == ID)
    {
        parse_id_list(id_type);
    }
    else if (token2.token_type == ARRAY || token2.token_type == LBRACE)
    {
        return;
    }
    else
    {
        syntax_error();
    }
}

void Parser::parse_block()
{
    expect(LBRACE);
    parse_statement_list();
    expect(RBRACE);
}

void Parser::parse_statement_list()
{
    program_statement_table.push_back(parse_statement());
    Token token;
    token = lexer.peek(1);

    if (token.token_type == OUTPUT || token.token_type == ID)
    {
        parse_statement_list();
    }
    else if (token.token_type == RBRACE)
    {
        return;
    }
    else
    {
        syntax_error();
    }
}

struct program_graph_node *Parser::parse_statement()
{
    Token token;
    token = lexer.peek(1);
    struct program_graph_node *expression_node;
    if (token.token_type == ID)
    {
        expression_node = parse_assignment_statement();
    }
    else if (token.token_type == OUTPUT)
    {
        expression_node = parse_output_statement();
    }
    else
    {
        syntax_error();
    }

    return expression_node;
}

struct program_graph_node *Parser::parse_assignment_statement()
{
    struct program_graph_node *complete_assignment_tree = new program_graph_node;
    complete_assignment_tree->operation = "=";

    struct program_graph_node *left_hand_side;
    left_hand_side = parse_variable_access(0);
    Token t = expect(EQUAL);
    complete_assignment_tree->line_no = t.line_no;

    struct program_graph_node *right_hand_side;
    right_hand_side = parse_expression(0);

    complete_assignment_tree->lhs = left_hand_side;
    complete_assignment_tree->rhs = right_hand_side;

    expect(SEMICOLON);

    return complete_assignment_tree;
}

struct program_graph_node *Parser::parse_output_statement()
{
    struct program_graph_node *complete_output_tree;
    Token token = expect(OUTPUT);

    complete_output_tree = parse_variable_access(1);
    complete_output_tree->line_no = token.line_no;
    complete_output_tree->assign = 1;

    expect(SEMICOLON);

    return complete_output_tree;
}

struct program_graph_node *Parser::parse_variable_access(
    int check_assignment_state)
{
    Token token1;
    Token token2;
    Token token3;

    token1 = expect(ID);
    token2 = lexer.peek(1);
    token3 = lexer.peek(2);

    struct program_graph_node *variable_tree = new program_graph_node;

    if (token2.token_type == LBRAC && token3.token_type == DOT)
    {
        expect(LBRAC);
        expect(DOT);
        expect(RBRAC);
        variable_tree->operation = "[.]";

        struct program_graph_node *id_tree = new program_graph_node;
        id_tree->token_term = token1;
        variable_tree->lhs = id_tree;
    }
    else if (token2.token_type == LBRAC &&
             (token3.token_type == ID || token3.token_type == NUM ||
              token3.token_type == LPAREN))
    {
        variable_tree->operation = "[]";
        struct program_graph_node *id_tree = new program_graph_node;
        id_tree->token_term = token1;
        variable_tree->lhs = id_tree;

        expect(LBRAC);

        if (check_assignment_state == 0)
        {
            variable_tree->rhs = parse_expression(1);
        }
        else
        {
            variable_tree->rhs = parse_expression(2);
        }

        expect(RBRAC);
    }
    else if (token2.token_type == EQUAL || token2.token_type == SEMICOLON)
    {
        variable_tree->operation = "";
        variable_tree->token_term = token1;
    }
    else
    {
        syntax_error();
    }
    return variable_tree;
}

Token Parser::check_end(int check_assignment_state)
{
    Token token1;
    Token token2;
    Token temp_token;

    token1 = lexer.peek(1);
    token2 = lexer.peek(2);

    temp_token.lexeme = "";
    temp_token.token_type = END_OF_FILE;

    if (check_assignment_state == 0)
    {
        if (token1.token_type == SEMICOLON)
        {
            return temp_token;
        }
    }
    else if (check_assignment_state == 1)
    {
        if (token1.token_type == RBRAC && token2.token_type == EQUAL)
        {
            return temp_token;
        }
    }
    else if (check_assignment_state == 2)
    {
        if (token1.token_type == RBRAC && token2.token_type == SEMICOLON)
        {
            return temp_token;
        }
    }

    return token1;
}

struct stack_of_graph_nodes *Parser::find_top_token(
    std::stack<struct stack_of_graph_nodes *> stk)
{
    std::stack<struct stack_of_graph_nodes *> temp_stack;
    struct stack_of_graph_nodes *token_term;

    while (!stk.empty())
    {
        if (stk.top()->statement_type == "TERM")
        {
            token_term = stk.top();

            while (!temp_stack.empty())
            {
                stk.push(temp_stack.top());
                temp_stack.pop();
            }

            return token_term;
        }
        else
        {
            temp_stack.push(stk.top());
            stk.pop();
        }
    }
    return NULL;
}

struct program_graph_node *Parser::parse_expression(
    int check_assignment_state)
{
    std::stack<struct stack_of_graph_nodes *> stk;
    struct stack_of_graph_nodes *temp_stack = new stack_of_graph_nodes;
    Token token;
    std::string buildexpr = "";

    temp_stack->statement_type = "TERM";
    token.lexeme = "";
    token.token_type = END_OF_FILE;
    temp_stack->token_term = token;
    stk.push(temp_stack);

    struct stack_of_graph_nodes *top_token = find_top_token(stk);
    std::string first =
        all_grammar_keys_for_comparison[top_token->token_term.token_type];

    Token next_term;
    next_term = lexer.peek(1);
    std::string second = all_grammar_keys_for_comparison[next_term.token_type];

    if (valid_code_grammar_strings.find(second) ==
        valid_code_grammar_strings.end())
    {
        syntax_error();
    }

    while (operator_precedence_table[first][second] != 4)
    {
        std::vector<struct stack_of_graph_nodes *> stk1;

        if (operator_precedence_table[first][second] == 1 ||
            operator_precedence_table[first][second] == 2)
        {
            Token put = lexer.GetToken();
            struct stack_of_graph_nodes *new_temp = new stack_of_graph_nodes;
            new_temp->statement_type = "TERM";
            new_temp->token_term = put;
            stk.push(new_temp);
        }
        else if (operator_precedence_table[first][second] == 0)
        {
            std::string buildexpr = "";

            while (operator_precedence_table[first][second] != 1)
            {
                if (operator_precedence_table[first][second] == 3)
                    syntax_error();

                while (stk.top()->statement_type != "TERM")
                {
                    stk1.insert(stk1.begin(), stk.top());
                    buildexpr = stk.top()->statement_type + buildexpr;
                    stk.pop();
                }

                struct stack_of_graph_nodes *secondstack;
                secondstack = stk.top();
                second =
                    all_grammar_keys_for_comparison[stk.top()->token_term.token_type];
                stk.pop();
                first = all_grammar_keys_for_comparison[find_top_token(stk)
                                                            ->token_term.token_type];
                stk1.insert(stk1.begin(), secondstack);
                buildexpr = second + buildexpr;
            }

            while (stk.top()->statement_type != "TERM")
            {
                stk1.insert(stk1.begin(), stk.top());
                buildexpr = stk.top()->statement_type + buildexpr;
                stk.pop();
            }

            if (valid_statement_grammar_strings.find(buildexpr) ==
                valid_statement_grammar_strings.end())
            {
                syntax_error();
            }

            struct program_graph_node *expr = new program_graph_node;
            std::pair<std::string, std::vector<int>> build =
                grammar_expression_to_string_table[buildexpr];
            expr->operation = build.first;

            if (buildexpr == "ID" || buildexpr == "NUM")
            {
                expr->token_term = stk1[build.second[0]]->token_term;
                struct stack_of_graph_nodes *new_temp = new stack_of_graph_nodes;
                new_temp->statement_type = "EXPR";
                new_temp->expr = expr;
                stk.push(new_temp);
            }
            else if (buildexpr == "LPARENEXPRRPAREN")
            {
                stk.push(stk1[build.second[0]]);
            }
            else
            {
                if (build.second.size() == 2)
                {
                    expr->lhs = stk1[build.second[0]]->expr;
                    expr->rhs = stk1[build.second[1]]->expr;
                }
                else
                {
                    expr->lhs = stk1[build.second[0]]->expr;
                }

                struct stack_of_graph_nodes *new_temp = new stack_of_graph_nodes;
                new_temp->statement_type = "EXPR";
                new_temp->expr = expr;
                stk.push(new_temp);
            }
        }
        else if (operator_precedence_table[first][second] == 3)
        {
            syntax_error();
        }

        if (check_end(check_assignment_state).token_type == END_OF_FILE)
        {
            second = "END_OF_FILE";
        }
        else
        {
            next_term = lexer.peek(1);
            second = all_grammar_keys_for_comparison[next_term.token_type];

            if (valid_code_grammar_strings.find(second) ==
                valid_code_grammar_strings.end())
            {
                syntax_error();
            }
        }

        top_token = find_top_token(stk);
        first = all_grammar_keys_for_comparison[top_token->token_term.token_type];
    }

    return stk.top()->expr;
}

void Parser::breadth_first_search(struct program_graph_node *first_graph_node,
                                  bool print_statement)
{
    std::queue<struct program_graph_node *> queue_of_graph_nodes;
    queue_of_graph_nodes.push(first_graph_node);
    bool first = true;

    while (!queue_of_graph_nodes.empty())
    {
        int queue_size = queue_of_graph_nodes.size();

        while (queue_size > 0)
        {
            struct program_graph_node *temp_node = queue_of_graph_nodes.front();
            queue_of_graph_nodes.pop();

            if (temp_node->lhs == NULL && temp_node->rhs == NULL)
            {
                if (temp_node->token_term.token_type == NUM)
                {
                    temp_node->statement_type = "SCALAR";
                }
                else
                {
                    if (scalar_variables.find(temp_node->token_term.lexeme) !=
                        scalar_variables.end())
                    {
                        temp_node->statement_type = "SCALAR";
                    }
                    else if (array_variables.find(temp_node->token_term.lexeme) !=
                             array_variables.end())
                    {
                        temp_node->statement_type = "ARRAYDECL";
                    }
                    else
                    {
                        temp_node->statement_type = "ERROR";
                    }
                }

                if (print_statement)
                {
                    if (first)
                    {
                        std::cout << all_grammar_keys_for_comparison[temp_node->token_term
                                                                         .token_type]
                                  << " \"" << temp_node->token_term.lexeme << "\"";
                        first = false;
                    }
                    else
                    {
                        std::cout << " "
                                  << all_grammar_keys_for_comparison[temp_node->token_term
                                                                         .token_type]
                                  << " \"" << temp_node->token_term.lexeme << "\"";
                    }
                }
            }
            else
            {
                if (print_statement)
                {
                    if (first)
                    {
                        std::cout << temp_node->operation;
                        first = false;
                    }
                    else
                    {
                        std::cout << " " << temp_node->operation;
                    }
                }

                if (temp_node->lhs != NULL)
                {
                    queue_of_graph_nodes.push(temp_node->lhs);
                }
                if (temp_node->rhs != NULL)
                {
                    queue_of_graph_nodes.push(temp_node->rhs);
                }
            }
            queue_size--;
        }
    }
    std::cout << std::endl;
}

void Parser::task1()
{
    // Passing only the first element of the program graph as the output has to be
    // only for the first statement of the program.
    breadth_first_search(program_statement_table[0], true);
}

std::string Parser::depth_first_search(struct program_graph_node *root)
{
    if (root->lhs == NULL && root->rhs == NULL)
    {
        return root->statement_type;
    }

    std::string left = "";
    std::string right = "";
    std::string return_type = "";

    if (root->operation != "[.]")
    {
        left = depth_first_search(root->lhs);
        right = depth_first_search(root->rhs);
    }
    else
    {
        left = depth_first_search(root->lhs);
    }

    if (left == "ERROR" || right == "ERROR")
    {
        return_type = "ERROR";
    }

    if (root->operation == "+" || root->operation == "-")
    {
        if (left == "ARRAY" && right == "ARRAY")
            return_type = "ARRAY";
        else if (left == "SCALAR" && right == "SCALAR")
            return_type = "SCALAR";
        else
            return_type = "ERROR";
    }

    if (root->operation == "*")
    {
        if (left == "ARRAY" && right == "ARRAY")
            return_type = "SCALAR";
        else if (left == "SCALAR" && right == "SCALAR")
            return_type = "SCALAR";
        else
            return_type = "ERROR";
    }

    if (root->operation == "/")
    {
        if (left == "SCALAR" && right == "SCALAR")
            return_type = "SCALAR";
        else
            return_type = "ERROR";
    }

    if (root->operation == "[]")
    {
        if ((left == "ARRAYDECL" || left == "ARRAY") && right == "SCALAR")
            return_type = "SCALAR";
        else
            return_type = "ERROR";
    }

    if (root->operation == "[.]")
    {
        if (left == "ARRAYDECL" || left == "SCALAR")
            return_type = "ARRAY";
        else
            return_type = "ERROR";
    }
    root->statement_type = return_type;
    return return_type;
}

// Task 2
void Parser::task2()
{
    for (int i = 0; i < program_statement_table.size(); i++)
    {
        struct program_graph_node *temp = program_statement_table[i];
        breadth_first_search(program_statement_table[i], false);
        if (temp->assign == 0)
        {
            std::string left;
            if (temp->lhs->operation == "[]")
            {
                left = depth_first_search(temp->lhs->rhs);
                if (!(temp->lhs->lhs->statement_type == "ARRAYDECL" &&
                      left == "SCALAR"))
                    left = "ERROR";
                else
                    left = "SCALAR";
            }
            else if (temp->lhs->operation == "[.]")
            {
                if (!(temp->lhs->lhs->statement_type == "ARRAYDECL"))
                    left = "ERROR";
                else
                    left = "ARRAY";
            }
            else
            {
                if (temp->lhs->statement_type == "SCALAR" && temp->lhs->operation == "")
                    left = "SCALAR";
                else
                    left = "ERROR";
            }
            std::string right = depth_first_search(temp->rhs);
            if (left == "ERROR" || right == "ERROR" || left == "ARRAYDECL" ||
                right == "ARRAYDECL")
            {
                typelino.push_back(temp->line_no);
            }
            else
            {
                if (left == "SCALAR")
                {
                    if (right != "SCALAR")
                        assignlino.push_back(temp->line_no);
                }
            }
        }
        else
        {
            std::string left;
            if (temp->operation == "" && temp->statement_type == "SCALAR")
            {
                left = "SCALAR";
            }
            else if (temp->operation == "[]" &&
                     temp->lhs->statement_type == "ARRAYDECL")
            {
                if (depth_first_search(temp->rhs) == "SCALAR")
                {
                    left = "SCALAR";
                }
                else
                {
                    left = "ERROR";
                }
            }
            else if (temp->operation == "[.]" &&
                     temp->lhs->statement_type == "ARRAYDECL")
            {
                left = "ARRAY";
            }
            else
            {
                left = "ERROR";
            }
            if (left == "ERROR")
                typelino.push_back(temp->line_no);
        }
    }
    if (typelino.size() > 0)
    {
        std::cout << "Disappointing expression type error :(" << std::endl;
        std::cout << std::endl;
        for (int i = 0; i < typelino.size(); i++)
        {
            std::cout << "Line " << typelino[i] << std::endl;
        }
        exit(1);
    }
    else
    {
        if (assignlino.size() > 0)
        {
            std::cout << "The following assignment(s) is/are invalid :(" << std::endl;
            std::cout << std::endl;
            for (int i = 0; i < assignlino.size(); i++)
            {
                std::cout << "Line " << assignlino[i] << std::endl;
            }
            exit(1);
        }
        else
        {
            std::cout << "Amazing! No type errors here :)" << std::endl;
        }
    }
}

// Task 1
void parse_and_generate_AST()
{
    Parser p;
    p.parse_program();
    p.task1();
}

// Task 2
void parse_and_type_check()
{
    Parser p;
    p.parse_program();
    p.task2();
}

// Task 3
instNode *parse_and_generate_statement_list()
{
    std::cout << "3" << std::endl;

    // The following is the hardcoded statement list
    // generated for a specific program
    // you should replace this code with code that parses the
    // input and generayes a statement list
    //
    // program
    // SCALAR a b c d
    // ARRAY x y z
    // a = 1;
    // b = 2;
    // c = 3;
    // d = (a+b)*(b+c);
    // OUTPUT d;
    // x[a+b] = d;
    // OUTPUT x[3];
    //
    //  a will be at location 0
    //  b will be at location 1
    //  c will be at location 2
    //  d will be at location 3
    //  x will be at location 4 - 13
    //  y will be at location 14 - 23
    //  z will be at location 24 - 33
    //  t1 will be at location 34 : intermediate value for (a+b)
    //  t2 will be at location 35 : intermediate value for (b+c)
    //  t3 will be at location 36 : intermediate value (a+b)*(b+c)
    //  t4 will be at location 37 : intermediate value for a+b index of array
    //  t5 will be at location 38 : intermediate value for addr of x[a+b] =
    //                              address_of_x + value of a+b =
    //                              4 + value of a+b
    //  t6 will be at location 39 : intermediate value for addr of x[3] =
    //                              address_of_x + value of 3 =
    //                              4 + value of 3 (computation is not done at
    //                              compile time)
    //
    instNode *i01 = new instNode();
    i01->lhsat = DIRECT;
    i01->lhs = 0;             // a
    i01->iType = ASSIGN_INST; // =
    i01->op1at = IMMEDIATE;
    i01->op1 = 1;        // 1
    i01->oper = OP_NOOP; // no operator

    instNode *i02 = new instNode();
    i02->lhsat = DIRECT;
    i02->lhs = 1;             // b
    i02->iType = ASSIGN_INST; // =
    i02->op1at = IMMEDIATE;
    i02->op1 = 2;        // 2
    i02->oper = OP_NOOP; // no operator

    i01->next = i02;

    instNode *i03 = new instNode();
    i03->lhsat = DIRECT;
    i03->lhs = 2;             // c
    i03->iType = ASSIGN_INST; // =
    i03->op1at = IMMEDIATE;
    i03->op1 = 3;        // 3
    i03->oper = OP_NOOP; // no operator

    i02->next = i03;

    instNode *i1 = new instNode();
    i1->lhsat = DIRECT;
    i1->lhs = 34;            // t1
    i1->iType = ASSIGN_INST; // =
    i1->op1at = DIRECT;
    i1->op1 = 0;        // a
    i1->oper = OP_PLUS; // +
    i1->op2at = DIRECT;
    i1->op2 = 1; // b

    i03->next = i1;

    instNode *i2 = new instNode();
    i2->lhsat = DIRECT;
    i2->lhs = 35;            // t2
    i2->iType = ASSIGN_INST; // =
    i2->op1at = DIRECT;
    i2->op1 = 1;        // b
    i2->oper = OP_PLUS; // +
    i2->op2at = DIRECT;
    i2->op2 = 2; // c

    i1->next = i2;

    instNode *i3 = new instNode();
    i3->lhsat = DIRECT;
    i3->lhs = 36;            // t3
    i3->iType = ASSIGN_INST; // =
    i3->op1at = DIRECT;
    i3->op1 = 34;       // t1
    i3->oper = OP_MULT; // *
    i3->op2at = DIRECT;
    i3->op2 = 35; // t2

    i2->next = i3; // i3 should be after i1 and i2

    instNode *i4 = new instNode();
    i4->lhsat = DIRECT;
    i4->lhs = 3;             // d
    i4->iType = ASSIGN_INST; // =
    i4->op1at = DIRECT;
    i4->op1 = 36;       // t3
    i4->oper = OP_NOOP; // no operator

    i3->next = i4;

    instNode *i5 = new instNode();
    i5->iType = OUTPUT_INST; // OUTPUT
    i5->op1at = DIRECT;
    i5->op1 = 3; // d

    i4->next = i5;

    instNode *i6 = new instNode();
    i6->lhsat = DIRECT;
    i6->lhs = 37;            // t4
    i6->iType = ASSIGN_INST; // =
    i6->op1at = DIRECT;
    i6->op1 = 0;        // a
    i6->oper = OP_PLUS; // +
    i6->op2at = DIRECT;
    i6->op2 = 1; // b
    i5->next = i6;

    instNode *i7 = new instNode();
    i7->lhsat = DIRECT;
    i7->lhs = 38;            // t5
    i7->iType = ASSIGN_INST; // =
    i7->op1at = IMMEDIATE;
    i7->op1 = 4;        // address of x = 4 available
                        // at compile time
    i7->oper = OP_PLUS; // +
    i7->op2at = DIRECT;
    i7->op2 = 37; // t5 (contains value of a+b

    i6->next = i7;

    instNode *i8 = new instNode();
    i8->lhsat = INDIRECT;
    i8->lhs = 38;            // x[a+b]
    i8->iType = ASSIGN_INST; // =
    i8->op1at = DIRECT;
    i8->op1 = 3; // d
    i8->oper = OP_NOOP;

    i7->next = i8;

    instNode *i9 = new instNode();
    i9->lhsat = DIRECT;
    i9->lhs = 39;            // t6 will contain address of x[3]
    i9->iType = ASSIGN_INST; // =
    i9->op1at = IMMEDIATE;
    i9->op1 = 4;        // address of x = 4 available
                        // at compile time
    i9->oper = OP_PLUS; // +
    i9->op2at = IMMEDIATE;
    i9->op2 = 3; // 3

    i8->next = i9;

    instNode *i10 = new instNode();
    i10->iType = OUTPUT_INST; // OUTPUT
    i10->op1at = INDIRECT;
    i10->op1 = 39; // x[3] by providing its
                   // address indirectly through
                   // t6

    i9->next = i10;

    instNode *code = i01;

    return code;
}
