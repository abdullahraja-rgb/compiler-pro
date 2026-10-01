#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "ast.hpp"

namespace Tacky {

using Identifier = std::string;

enum class UnaryOperator {
    Complement,
    Negate,
    Not 
};

enum class BinaryOperator {
    Add,
    Subtract,
    Multiply, 
    Divide,
    Remainder, 
    Equal,
    NotEqual,
    LessThan,
    LessOrEqual,
    GreaterThan,
    GreaterOrEqual
};


// val = Constant(int) | Var(identifier)
struct Value {
    virtual ~Value() = default;
};

struct ConstantValue : Value {
    int value;

    explicit ConstantValue(int value)
        : value(value) {}
};

struct VariableValue : Value {
    Identifier name;

    explicit VariableValue(Identifier name)
        : name(std::move(name)) {}
};

// instruction = Return(val)
//             | Unary(unary_operator, val src, val dst)
struct Instruction {
    virtual ~Instruction() = default;
};

struct ReturnInstruction : Instruction {
    std::unique_ptr<Value> value;

    explicit ReturnInstruction(std::unique_ptr<Value> value)
        : value(std::move(value)) {}
};

struct UnaryInstruction : Instruction {
    UnaryOperator unary_operator;
    std::unique_ptr<Value> source;
    std::unique_ptr<Value> destination;

    UnaryInstruction(
        UnaryOperator unary_operator,
        std::unique_ptr<Value> source,
        std::unique_ptr<Value> destination
    )
        : unary_operator(unary_operator),
          source(std::move(source)),
          destination(std::move(destination)) {}
};

struct BinaryInstruction : Instruction {
    BinaryOperator binop;
    std::unique_ptr<Value> src1;
    std::unique_ptr<Value> src2;
    std::unique_ptr<Value> dst;

    BinaryInstruction(
        BinaryOperator binop,
        std::unique_ptr<Value> src1,
        std::unique_ptr<Value> src2,
        std::unique_ptr<Value> dst
    )
        : binop(binop),
          src1(std::move(src1)),
          src2(std::move(src2)),
          dst(std::move(dst)) {}
};

struct Copy : Instruction {
    std::unique_ptr<Value> src;
    std::unique_ptr<Value> dst;

    Copy(
        std::unique_ptr<Value> src,
        std::unique_ptr<Value> dst
    )   
        : src(std::move(src)),
          dst(std::move(dst)) {}

};

struct Jump : Instruction {
    Identifier target;

    Jump (
        Identifier target
    ) 
        : target(std::move(target)) {}
};

struct JumpIfZero : Instruction {
    std::unique_ptr<Value> condition;
    Identifier target;

    JumpIfZero(
        std::unique_ptr<Value> condition,
        Identifier target
    )
        : condition(std::move(condition)),
          target(std::move(target)) {}
};


struct JumpIfNotZero: Instruction {
    std::unique_ptr<Value> condition;
    Identifier target;

    JumpIfNotZero(
        std::unique_ptr<Value> condition,
        Identifier target
    )
        : condition(std::move(condition)),
          target(std::move(target)) {}
};

struct Label : Instruction {
    Identifier name;

    explicit Label(Identifier name)
        : name(std::move(name)) {}
};


// function_definition = Function(identifier, instruction* body)
struct FunctionDefinition {
    Identifier name;
    std::vector<std::unique_ptr<Instruction>> body;

    FunctionDefinition(
        Identifier name,
        std::vector<std::unique_ptr<Instruction>> body
    )
        : name(std::move(name)),
          body(std::move(body)) {}
};

// program = Program(function_definition)
struct Program {
    FunctionDefinition function;

    explicit Program(FunctionDefinition function)
        : function(std::move(function)) {}
};

} // namespace Tacky


struct TackyGenerator {

    Tacky::Program generate_program(const Program& program);

    Tacky::FunctionDefinition generate_function(
        const FunctionDefinition& function
    );

    std::vector<std::unique_ptr<Tacky::Instruction>>
    generate_instructions(const Statement& statement);

    std::unique_ptr<Tacky::Value> generate_value(
        const Expression& expression,
        std::vector<std::unique_ptr<Tacky::Instruction>>& instructions
    );

    std::unique_ptr<Tacky::Value> generate_logical_and(
        const BinaryExpression& expression,
        std::vector<std::unique_ptr<Tacky::Instruction>>& instructions
    );

    std::unique_ptr<Tacky::Value> generate_logical_or(
        const BinaryExpression& expression,
        std::vector<std::unique_ptr<Tacky::Instruction>>& instructions
    );

    Tacky::UnaryOperator generate_unop(
        ::UnaryOperator ast_operator
    );

    Tacky::BinaryOperator generate_binop(
        ::BinaryOperator ast_bi_operator
    );

    // prevent conflict in generated ids
    std::string make_temporary();
    int temporary_counter = 0;
};
