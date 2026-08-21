// Placeholder for the code generation interface.
// Codegen will eventually turn compiler IR or AST nodes into assembly.

/*
program = Program(function_definition)
function_definition = Function(identifier name, instruction* instructions)
instruction = Mov(operand src, operand dst)
| Unary(unary_operator, operand)
| Binary(binary_operator, operand, operand)
| Idiv(operand)
| Cdq
| AllocateStack(int)
| Ret
unary_operator = Neg | Not
binary_operator = Add | Sub | Mult
operand = Imm(int) | Reg(reg) | Pseudo(identifier) | Stack(int)
reg = AX | DX | R10 | R11
*/


#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "tacky.hpp"




namespace Assembly {

enum class UnaryOperator {
    Neg,
    Not,
};

enum class Register {
    AX,
    R10,
    DX,
    R11
};

enum class BinaryOperator {
    Add,
    Sub,
    Mult
};

// ASDL: operand = Imm(int) | Reg(reg) | Pseudo(identifier) | Stack(int)
struct Operand {
    virtual ~Operand() = default;
};

struct ImmOperand : Operand {
    int value;

    explicit ImmOperand(int value)
        : value(value) {}
};

struct RegisterOperand : Operand {
    Register reg;

    explicit RegisterOperand(Register reg)
        : reg(reg) {}
};

struct PseudoOperand : Operand {
    std::string identifier;

    explicit PseudoOperand(std::string identifier)
        : identifier(std::move(identifier)) {}
};

struct StackOperand : Operand {
    int offset;

    explicit StackOperand(int offset)
        : offset(offset) {}
};

// ASDL: instruction = Mov(...) | Unary(...) | AllocateStack(int) | Ret
struct Instruction {
    virtual ~Instruction() = default;
};

struct MovInstruction : Instruction {
    std::unique_ptr<Operand> src;
    std::unique_ptr<Operand> dst;

    MovInstruction(std::unique_ptr<Operand> src, std::unique_ptr<Operand> dst)
        : src(std::move(src)), dst(std::move(dst)) {}
};

struct UnaryInstruction : Instruction {
    UnaryOperator unary_operator;
    std::unique_ptr<Operand> operand;

    UnaryInstruction(
        UnaryOperator unary_operator,
        std::unique_ptr<Operand> operand
    )
        : unary_operator(unary_operator),
          operand(std::move(operand)) {}
};

struct BinaryInstruction : Instruction {
    BinaryOperator binary_operator;
    std::unique_ptr<Operand> operand1;
    std::unique_ptr<Operand> operand2;

    BinaryInstruction (
        BinaryOperator binary_operator,
        std::unique_ptr<Operand> operand1,
        std::unique_ptr<Operand> operand2

    )
        : binary_operator(binary_operator),
          operand1(std::move(operand1)),
          operand2(std::move(operand2)) {}
};

struct Idiv : Instruction {
    std::unique_ptr<Operand> operand;

    Idiv (
        std::unique_ptr<Operand> operand

    )
        : operand(std::move(operand)) {}
};



struct AllocateStackInstruction : Instruction {
    int bytes;

    explicit AllocateStackInstruction(int bytes)
        : bytes(bytes) {}
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

    Assembly::Program generate_program(const Tacky::Program& program);

    Assembly::FunctionDefinition generate_function(const Tacky::FunctionDefinition& function);

    std::vector<std::unique_ptr<Assembly::Instruction>> generate_instructions(
        const std::vector<std::unique_ptr<Tacky::Instruction>>& instructions
    );

    std::vector<std::unique_ptr<Assembly::Instruction>> generate_instruction(
        const Tacky::Instruction& instruction
    );

    std::unique_ptr<Assembly::Operand> generate_operand(
        const Tacky::Value& value
    );

    Assembly::UnaryOperator generate_unop(
        Tacky::UnaryOperator unary_operator
    );

    int replace_pseudos(
        std::vector<std::unique_ptr<Assembly::Instruction>>& instructions
    );

    std::vector<std::unique_ptr<Assembly::Instruction>> fix_instructions(
        std::vector<std::unique_ptr<Assembly::Instruction>> instructions
    );

};

struct AssemblyEmitter {

    std::string emit_program(const Assembly::Program& program);

    std::string emit_function(const Assembly::FunctionDefinition& function);

    std::string emit_instruction(const Assembly::Instruction& statement);

    std::string emit_operand(const Assembly::Operand& expression);

};
