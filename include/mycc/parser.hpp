// Placeholder for the parser interface.
// The parser will eventually turn tokens into an abstract syntax tree.


#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
// for the nodes 
#include "ast.hpp"
// for the token struct
#include "lexer.hpp"

// we would need functions for each non terminal
/*  
Program parse_program(std::vector<Token> tokens);
FunctionDefinition parse_function(std::vector<Token> tokens);
Statement parse_statement(std::vector<Token> tokens);
Expression parse_exp(std::vector<Token> tokens);
Identifier parse_id(std::vector<Token> tokens);
ConstantExpression parse_int(std::vector<Token> tokens);

Token expected(const std::string& expectedtype);
*/

struct Parser {

    const std::vector<Token>& tokens;
    size_t currIdx = 0;

    Parser(const std::vector<Token>& tokens)
        : tokens(tokens) {}

    Program parse_program();
    FunctionDefinition parse_function();
    std::unique_ptr<Statement> parse_statement();
    std::unique_ptr<Expression> parse_expression();
    Identifier parse_id();
    std::unique_ptr<ConstantExpression> parse_int();

    Token expected(const std::string& expectedtype);

};


