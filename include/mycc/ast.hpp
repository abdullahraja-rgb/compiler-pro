// Placeholder for AST definitions.
// The AST will eventually represent the structure of parsed source programs.

/*
program = Program(function_definition)
function_definition = Function(identifier name, statement body)
statement = Return(exp)
exp = Constant(int) Unary(unary_operator, exp)
unary_operator = Complement | Negate
 */

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

using Identifier = std::string;

// unary_operator = Complement | Negate (two labels w no input)
enum UnaryOperator {
    Complement,
    Negate,
    Not
};

enum BinaryOperator {
    Add,
    Subtract,
    Multiply, 
    Divide,
    Remainder,
    And,
    Or,
    Equal,
    NotEqual,
    LessThan,
    LessOrEqual,
    GreaterThan,
    GreaterOrEqual
};


// ASDL: exp = Constant(int)
struct Expression {
    virtual ~Expression() = default;
};

struct ConstantExpression : Expression {
    int value;

    explicit ConstantExpression(int value) : value(value) {}
};

struct VariableExpression : Expression {
    Identifier name;

    explicit VariableExpression(Identifier name)
        : name(std::move(name)) {}
};

struct UnaryExpression : Expression {
    UnaryOperator unary_operator;
    std::unique_ptr<Expression> value;

    UnaryExpression(
        UnaryOperator unary_operator,
        std::unique_ptr<Expression> value
    )
        : unary_operator(unary_operator),
          value(std::move(value)) {}
};

struct BinaryExpression : Expression {
    BinaryOperator binary_operator;
    std::unique_ptr<Expression> exp1;
    std::unique_ptr<Expression> exp2;

    BinaryExpression(
        BinaryOperator binary_operator,
        std::unique_ptr<Expression> exp1,
        std::unique_ptr<Expression> exp2
    )
        : binary_operator(binary_operator), 
          exp1(std::move(exp1)),
          exp2(std::move(exp2)) {}
};

struct AssignmentExpression : Expression {
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    AssignmentExpression(
        std::unique_ptr<Expression> left,
        std::unique_ptr<Expression> right
    )
        : left(std::move(left)),
          right(std::move(right)) {}
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

struct ExpressionStatement : Statement {
    std::unique_ptr<Expression> expression;

    explicit ExpressionStatement(std::unique_ptr<Expression> expression)
        : expression(std::move(expression)) {}
};

struct NullStatement : Statement {};

struct Declaration {
    Identifier name;
    std::unique_ptr<Expression> initializer;

    Declaration(
        Identifier name,
        std::unique_ptr<Expression> initializer
    )
        : name(std::move(name)),
          initializer(std::move(initializer)) {}
};

struct BlockItem {
    virtual ~BlockItem() = default;
};

struct StatementBlockItem : BlockItem {
    std::unique_ptr<Statement> statement;

    explicit StatementBlockItem(std::unique_ptr<Statement> statement)
        : statement(std::move(statement)) {}
};

struct DeclarationBlockItem : BlockItem {
    Declaration declaration;

    explicit DeclarationBlockItem(Declaration declaration)
        : declaration(std::move(declaration)) {}
};

// ASDL: function_definition = Function(identifier name, block_item* body)
struct FunctionDefinition {
    Identifier name;
    std::vector<std::unique_ptr<BlockItem>> body;

    FunctionDefinition(
        Identifier name,
        std::vector<std::unique_ptr<BlockItem>> body
    )
        : name(std::move(name)), body(std::move(body)) {}
};

// ASDL: program = Program(function_definition)
struct Program {
    FunctionDefinition function;

    explicit Program(FunctionDefinition function)
        : function(std::move(function)) {}
};
