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

    std::unique_ptr<Expression> expression_parse = parse_expression(0);

    expected("Semicolon");

    return std::make_unique<ReturnStatement>(std::move(expression_parse));
}

std::unique_ptr<Expression> Parser::parse_factor() {
    const Token& next_token = peek();
    if (next_token.type == "Integer") {
        return parse_int();
    } else if (next_token.type == "Minus" || next_token.type == "Tilde") {
        UnaryOperator unop = parse_unop();
        std::unique_ptr<Expression> inner_expression = parse_factor(); 
        return std::make_unique<UnaryExpression>(unop, std::move(inner_expression));
    } else if (next_token.type == "LeftParen") {
        expected("LeftParen");
        std::unique_ptr<Expression> inner_expression = parse_expression(0);
        expected("RightParen");
        return inner_expression;
    } else {
        throw std::runtime_error(
            "Received an unexprected token" + next_token.value + " of type " + next_token.type
        );
    }   
}

std::unique_ptr<Expression> Parser::parse_expression(int min_prec) {
    std::unique_ptr<Expression> left = parse_factor();
    Token next_token = peek();
    while ((next_token.type == "Addition" || next_token.type == "Minus" || next_token.type == "Multiplication" || next_token.type == "Division" || next_token.type == "Remainder") && precedence(next_token) >= min_prec) {
        BinaryOperator bin_op = parse_binop();
        int input_minprec = precedence(next_token) + 1;
        std::unique_ptr<Expression> right = parse_expression(input_minprec);
        left = std::make_unique<BinaryExpression>(bin_op, std::move(left), std::move(right));
        next_token = peek();
    }
    return left;
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

BinaryOperator Parser::parse_binop() {
    const Token& token = peek();

    if (token.type == "Minus") {
        expected("Minus");
        return BinaryOperator::Subtract;
    }

    if (token.type == "Addition") {
        expected("Addition");
        return BinaryOperator::Add;
    }
    if (token.type == "Multiplication") {
        expected("Multiplication");
        return BinaryOperator::Multiply;
    }
    if (token.type == "Division") {
        expected("Division");
        return BinaryOperator::Divide;
    }
    if (token.type == "Remainder") {
        expected("Remainder");
        return BinaryOperator::Remainder;
    }

    throw std::runtime_error("Not correct input for the parse_binop()");
}

UnaryOperator Parser::parse_unop() {
    const Token& token = peek();

    if (token.type == "Minus") {
        expected("Minus");
        return UnaryOperator::Negate;
    }

    if (token.type == "Tilde") {
        expected("Tilde");
        return UnaryOperator::Complement;
    }

    throw std::runtime_error(
        "Parser error: expected unary operator but got '" +
        token.value + "'"
    );
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


const Token& Parser::peek() const {
    if (currIdx >= tokens.size()) {
        throw std::runtime_error(
            "Parser error: tried to peek past the end of tokens"
        );
    }

    return tokens[currIdx];
}

int Parser::precedence(const Token& token) {
    
    // const Token& token = peek();
    std::cout << token.type << std::endl;
    if (token.type == "Addition") {
        return 45;
    }
    if (token.type == "Minus") {
        return 45;
    }
    if (token.type == "Multiplication") {
        return 50;
    }
    if (token.type == "Division") {
        return 50;
    }
    if (token.type == "Remainder") {
        return 50;
    }
    throw std::runtime_error("Wrong Call");
}
