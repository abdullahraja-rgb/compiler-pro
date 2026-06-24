// Placeholder for parser implementation.
// First goal later: parse a single function with one return statement.


/* we are given a list of tokens now and we need to make use of serveral recursive functions 
to analyse the tokens and return the relevant class/node for each node representation */
#include "../include/mycc/parser.hpp"
#include <stdexcept>
#include <utility>


Program Parser::parse_program() {
    FunctionDefinition function_parse = parse_function();
    return Program(std::move(function_parse));
}

FunctionDefinition Parser::parse_function() {
    expected("KeywordInt");

    Identifier id_parse = parse_id();

    expected("LeftParen");

    expected("KeywordVoid");

    expected("RightParen");

    expected("LeftBrace");

    std::unique_ptr<Statement> statement_parse = parse_statement();

    expected("RightBrace");

    return FunctionDefinition(std::move(id_parse), std::move(statement_parse));
}

std::unique_ptr<Statement> Parser::parse_statement() {
    expected("KeywordReturn");

    std::unique_ptr<Expression> expression_parse = parse_expression();

    expected("Semicolon");

    return std::make_unique<ReturnStatement>(std::move(expression_parse));
}

std::unique_ptr<Expression> Parser::parse_expression() {
    return parse_int();
}

Identifier Parser::parse_id() {
    Token id_token = expected("Identifier");

    return id_token.value;
}

std::unique_ptr<ConstantExpression> Parser::parse_int() {
    Token int_token = expected("Integer");

    int int_value = std::stoi(int_token.value);

    return std::make_unique<ConstantExpression>(int_value);
}


Token Parser::expected(const std::string& expectedtype) {
    if (currIdx >= tokens.size()) {
        throw std::runtime_error("Parser error: reached end of tokens while expecting " + expectedtype);
    }

    Token current_token = tokens[currIdx];

    // if they dont match throw error
    if (current_token.type != expectedtype) {
        throw std::runtime_error("Parser error: expected " + expectedtype + " but got " + current_token.type);
    }

    ++currIdx;

    return current_token;
}
