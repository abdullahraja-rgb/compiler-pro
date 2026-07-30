// Placeholder for assembly code generation.
// First goal later: emit assembly that returns an integer exit code.

/* the very first step is to realise how the code in our c file looks like when translated to asssembly and what each section means 

int main(void) {
    return 42;
}

.globl main
main:
    movl $42, %eax
    ret

Program
  FunctionDefinition name=main
    ReturnStatement
      ConstantExpression value=42

program node -> calls the function
function def node -> assembler dir + main label
return statement -> calls expression
const expression -> movl... ->
return statement -> receives the expre and ret 

*/

#include <utility>
#include <stdexcept>
#include "../include/mycc/codegen.hpp"
#include <memory>
#include <string>
#include <vector>

/* needs to open the program node and call the generate function
function which will travrse the funcdef node */
Assembly::Program AssemblyGenerator::generate_program(const Program& program) {
    Assembly::FunctionDefinition func_generated = generate_function(program.function);

    return Assembly::Program(std::move(func_generated));
}

Assembly::FunctionDefinition AssemblyGenerator::generate_function(const FunctionDefinition& func_def) {
    std::string func_name = func_def.name;
    std::vector<std::unique_ptr<Assembly::Instruction>> inst_list = generate_statement(*func_def.body);
    return Assembly::FunctionDefinition(std::move(func_name), std::move(inst_list));
}

/* func_def.body is a statement node, the only statement we have is a return statement which is what we detect on detection we create the assembly instructions a combintion of mov and ret,   */
std::vector<std::unique_ptr<Assembly::Instruction>> AssemblyGenerator::generate_statement(const Statement& statement) {
    std::vector<std::unique_ptr<Assembly::Instruction>> instructions;
    
    // check if the input statement is a return via dynamic casting
    const ReturnStatement* rcv_statement = dynamic_cast<const ReturnStatement*>(&statement);
    // if casting didnt fail then its a return statement
    if (rcv_statement != nullptr) {
        // move the srcc=expression to dest which defs to eax
        /* access the value  */
        const Expression& expression = *rcv_statement->value;
        std::vector<std::unique_ptr<Assembly::Instruction>> express_val = generate_expression(expression);

        for (std::unique_ptr<Assembly::Instruction>& instruction : express_val) {
            instructions.push_back(std::move(instruction));
        }

        instructions.push_back(std::make_unique<Assembly::RetInstruction>());

        return instructions;
    }

    throw std::runtime_error("Unsupported statement in code generation");
}


std::vector<std::unique_ptr<Assembly::Instruction>> AssemblyGenerator::generate_expression(const Expression& expression) {
    std::vector<std::unique_ptr<Assembly::Instruction>> instructions;

    const ConstantExpression* const_expression = dynamic_cast<const ConstantExpression*>(&expression);

    if (const_expression != nullptr) {
        instructions.push_back(
            std::make_unique<Assembly::MovInstruction>(
                std::make_unique<Assembly::ImmOperand>(const_expression->value),
                std::make_unique<Assembly::RegisterOperand>("eax")
            )
        );

        return instructions;
    }

    throw std::runtime_error("Unsupported expression in code generation");
}


/*  */

std::string AssemblyEmitter::emit_program(const Assembly::Program& program) {
    std::string output = emit_function(program.function);
    return output;

}

std::string AssemblyEmitter::emit_function(const Assembly::FunctionDefinition& function) {
    std::string output;

    output += ".globl " + function.name + "\n";
    output += function.name + ":\n";

    for (const std::unique_ptr<Assembly::Instruction>& instruction : function.instructions) {
        output += emit_instruction(*instruction);
    }

    return output;
}

std::string AssemblyEmitter::emit_instruction(const Assembly::Instruction& instruction) {
    const Assembly::MovInstruction* mov_instruction = dynamic_cast<const Assembly::MovInstruction*>(&instruction);

    if (mov_instruction != nullptr) {
        std::string src = emit_operand(*mov_instruction->src);
        std::string dst = emit_operand(*mov_instruction->dst);

        return "    movl " + src + ", " + dst + "\n";
    }

    const Assembly::RetInstruction* ret_instruction = dynamic_cast<const Assembly::RetInstruction*>(&instruction);

    if (ret_instruction != nullptr) {
        return "    ret\n";
    }

    throw std::runtime_error("Unsupported instruction in assembly emitter");
}

std::string AssemblyEmitter::emit_operand(const Assembly::Operand& operand) {
    const Assembly::ImmOperand* imm_operand = dynamic_cast<const Assembly::ImmOperand*>(&operand);

    if (imm_operand != nullptr) {
        return "$" + std::to_string(imm_operand->value);
    }

    const Assembly::RegisterOperand* register_operand = dynamic_cast<const Assembly::RegisterOperand*>(&operand);

    if (register_operand != nullptr) {
        return "%" + register_operand->name;
    }

    throw std::runtime_error("Unsupported operand in assembly emitter");
}
