// Placeholder for AST definitions.
// The AST will eventually represent the structure of parsed source programs.

/*
program = Program(function_definition)
function_definition = Function(identifier name, statement body)
statement = Return(exp)
exp = Constant(int)
 */

#pragma once

#include <memory>
#include <string>
#include <utility>

using Identifier = std::string;

// ASDL: exp = Constant(int)
struct Expression {
    virtual ~Expression() = default;
};

struct ConstantExpression : Expression {
    int value;

    explicit ConstantExpression(int value) : value(value) {}
};

// ASDL: statement = Return(exp)
struct Statement {
    virtual ~Statement() = default;
};

struct ReturnStatement : Statement {
    std::unique_ptr<Expression> value;

    explicit ReturnStatement(std::unique_ptr<Expression> value)
        : value(std::move(value)) {}
};

// ASDL: function_definition = Function(identifier name, statement body)
struct FunctionDefinition {
    Identifier name;
    std::unique_ptr<Statement> body;

    FunctionDefinition(Identifier name, std::unique_ptr<Statement> body)
        : name(std::move(name)), body(std::move(body)) {}
};

// ASDL: program = Program(function_definition)
struct Program {
    FunctionDefinition function;

    explicit Program(FunctionDefinition function)
        : function(std::move(function)) {}
};

