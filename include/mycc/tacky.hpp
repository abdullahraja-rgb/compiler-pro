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
    Negate
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

    Tacky::UnaryOperator generate_unop(
        ::UnaryOperator ast_operator
    );

    // prevent conflict in generated ids
    std::string make_temporary();
    int temporary_counter = 0;
};