// Placeholder for the code generation interface.
// Codegen will eventually turn compiler IR or AST nodes into assembly.

/* 
program = Program(function_definition)
function_definition = Function(identifier name, instruction* instructions)
instruction = Mov(operand src, operand dst) | Ret
operand = Imm(int) | Register
 */


#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "ast.hpp"

namespace Assembly {

// ASDL: operand = Imm(int) | Register
struct Operand {
    virtual ~Operand() = default;
};

struct ImmOperand : Operand {
    int value;

    explicit ImmOperand(int value)
        : value(value) {}
};

struct RegisterOperand : Operand {
    std::string name;

    explicit RegisterOperand(std::string name)
        : name(std::move(name)) {}
};

// ASDL: instruction = Mov(operand src, operand dst) | Ret
struct Instruction {
    virtual ~Instruction() = default;
};

struct MovInstruction : Instruction {
    std::unique_ptr<Operand> src;
    std::unique_ptr<Operand> dst;

    MovInstruction(std::unique_ptr<Operand> src, std::unique_ptr<Operand> dst)
        : src(std::move(src)), dst(std::move(dst)) {}
};

struct RetInstruction : Instruction {};

// ASDL: function_definition = Function(identifier name, instruction* instructions)
struct FunctionDefinition {
    std::string name;
    std::vector<std::unique_ptr<Instruction>> instructions;

    FunctionDefinition(std::string name, std::vector<std::unique_ptr<Instruction>> instructions)
        : name(std::move(name)), instructions(std::move(instructions)) {}
};

// ASDL: program = Program(function_definition)
struct Program {
    FunctionDefinition function;

    explicit Program(FunctionDefinition function)
        : function(std::move(function)) {}
};

}

struct AssemblyGenerator {

    Assembly::Program generate_program(const Program& program);

    Assembly::FunctionDefinition generate_function(const FunctionDefinition& function);

    std::vector<std::unique_ptr<Assembly::Instruction>> generate_statement(const Statement& statement);

    std::vector<std::unique_ptr<Assembly::Instruction>> generate_expression(const Expression& expression);

};



