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


tacky

program = Program(function_definition)
function_definition = Function(identifier, instruction* body)
instruction =
      Return(val)
    | Unary(unary_operator, val src, val dst)
val =
      Constant(int)
    | Var(identifier)
unary_operator =
      Complement
    | Negate


assembly

program = Program(function_definition)
function_definition = Function(identifier name, instruction* instructions)
instruction =
      Mov(operand src, operand dst)
    | Unary(unary_operator, operand)
    | AllocateStack(int)
    | Ret
unary_operator =
      Neg
    | Not
operand =
      Imm(int)
    | Reg(reg)
    | Pseudo(identifier)
    | Stack(int)
reg =
      AX
    | R10


Program(function_definition) = Program(function_definition)
Function(name, instructions) = Function(name, instructions)
Return(val) = Mov(val, Reg(AX)), Ret
Unary(unary_operator, src, dst) =  Mov(src, dst), Unary(unary_operator, dst)
Complement = Not
Negate = Neg
Constant(int) = Imm(int)
Var(identifier) = Pseudo(identifier)

*/

#include <utility>
#include <stdexcept>
#include "../include/mycc/codegen.hpp"
#include "../include/mycc/tacky.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

/* needs to open the program node and call the generate function
function which will travrse the funcdef node */
Assembly::Program AssemblyGenerator::generate_program(const Tacky::Program& program) {
    Assembly::FunctionDefinition func_generated = generate_function(program.function);

    return Assembly::Program(std::move(func_generated));
}

Assembly::FunctionDefinition AssemblyGenerator::generate_function(const Tacky::FunctionDefinition& func_def) {
    std::string func_name = func_def.name;
    std::vector<std::unique_ptr<Assembly::Instruction>> inst_list =
        generate_instructions(func_def.body);

    int stack_size = replace_pseudos(inst_list);

    inst_list.insert(
        inst_list.begin(),
        std::make_unique<Assembly::AllocateStackInstruction>(stack_size)
    );
    std::vector<std::unique_ptr<Assembly::Instruction>> inst_list_final =
        fix_instructions(std::move(inst_list));

    return Assembly::FunctionDefinition(std::move(func_name), std::move(inst_list_final));
}

std::vector<std::unique_ptr<Assembly::Instruction>>
AssemblyGenerator::generate_instructions(
    const std::vector<std::unique_ptr<Tacky::Instruction>>& tacky_instructions
) {
    std::vector<std::unique_ptr<Assembly::Instruction>> instructions;

    for (const std::unique_ptr<Tacky::Instruction>& tacky_instruction : tacky_instructions) {
        std::vector<std::unique_ptr<Assembly::Instruction>> generated =
            generate_instruction(*tacky_instruction);

        for (std::unique_ptr<Assembly::Instruction>& instruction : generated) {
            instructions.push_back(std::move(instruction));
        }
    }

    return instructions;
}

std::vector<std::unique_ptr<Assembly::Instruction>>
AssemblyGenerator::generate_instruction(
    const Tacky::Instruction& tacky_instruction
) {
    std::vector<std::unique_ptr<Assembly::Instruction>> instructions;

    const Tacky::ReturnInstruction* return_instruction =
        dynamic_cast<const Tacky::ReturnInstruction*>(&tacky_instruction);

    if (return_instruction != nullptr) {
        instructions.push_back(
            std::make_unique<Assembly::MovInstruction>(
                generate_operand(*return_instruction->value),
                std::make_unique<Assembly::RegisterOperand>(Assembly::Register::AX)
            )
        );

        instructions.push_back(
            std::make_unique<Assembly::RetInstruction>()
        );

        return instructions;
    }

    const Tacky::UnaryInstruction* unary_instruction =
        dynamic_cast<const Tacky::UnaryInstruction*>(&tacky_instruction);

    if (unary_instruction != nullptr) {
        instructions.push_back(
            std::make_unique<Assembly::MovInstruction>(
                generate_operand(*unary_instruction->source),
                generate_operand(*unary_instruction->destination)
            )
        );

        instructions.push_back(
            std::make_unique<Assembly::UnaryInstruction>(
                generate_unop(unary_instruction->unary_operator),
                generate_operand(*unary_instruction->destination)
            )
        );

        return instructions;
    }

    const Tacky::BinaryInstruction* binary_instruction = dynamic_cast<
        const Tacky::BinaryInstruction*>(&tacky_instruction);
    
    if (binary_instruction != nullptr) {

        if (binary_instruction->binop == Tacky::BinaryOperator::Divide) {
            instructions.push_back(
                std::make_unique<Assembly::MovInstruction>(
                    generate_operand(*binary_instruction->src1),
                    std::make_unique<Assembly::RegisterOperand>(Assembly::Register::AX)
                )
            );

            instructions.push_back(
                std::make_unique<Assembly::Cdq>()
            );

            instructions.push_back(
                std::make_unique<Assembly::Idiv>(
                    generate_operand(*binary_instruction->src2)
                )
            );

            instructions.push_back(
                std::make_unique<Assembly::MovInstruction>(
                    std::make_unique<Assembly::RegisterOperand>(Assembly::Register::AX),
                    generate_operand(*binary_instruction->dst)
                )
            );

            return instructions;
        }

        if (binary_instruction->binop == Tacky::BinaryOperator::Remainder) {
            instructions.push_back(
                std::make_unique<Assembly::MovInstruction>(
                    generate_operand(*binary_instruction->src1),
                    std::make_unique<Assembly::RegisterOperand>(Assembly::Register::AX)
                )
            );

            instructions.push_back(
                std::make_unique<Assembly::Cdq>()
            );

            instructions.push_back(
                std::make_unique<Assembly::Idiv>(
                    generate_operand(*binary_instruction->src2)
                )
            );

            instructions.push_back(
                std::make_unique<Assembly::MovInstruction>(
                    std::make_unique<Assembly::RegisterOperand>(Assembly::Register::DX),
                    generate_operand(*binary_instruction->dst)
                )
            );

            return instructions;
        }

        instructions.push_back(
            std::make_unique<Assembly::MovInstruction>(
                generate_operand(*binary_instruction->src1),
                generate_operand(*binary_instruction->dst)
            )
        );

        instructions.push_back(
            std::make_unique<Assembly::BinaryInstruction>(
                generate_binop(binary_instruction->binop),
                generate_operand(*binary_instruction->src2),
                generate_operand(*binary_instruction->dst)
            )
        );

        return instructions;
    }

    throw std::runtime_error(
        "Unsupported TACKY instruction in assembly generation"
    );
}

std::unique_ptr<Assembly::Operand> AssemblyGenerator::generate_operand(
    const Tacky::Value& value
) {
    const Tacky::ConstantValue* constant_value =
        dynamic_cast<const Tacky::ConstantValue*>(&value);

    if (constant_value != nullptr) {
        return std::make_unique<Assembly::ImmOperand>(constant_value->value);
    }

    const Tacky::VariableValue* variable_value =
        dynamic_cast<const Tacky::VariableValue*>(&value);

    if (variable_value != nullptr) {
        return std::make_unique<Assembly::PseudoOperand>(
            variable_value->name
        );
    }

    throw std::runtime_error(
        "Unsupported TACKY value in assembly generation"
    );
}

Assembly::BinaryOperator AssemblyGenerator::generate_binop(
    Tacky::BinaryOperator binary_operator
) {
    if (binary_operator == Tacky::BinaryOperator::Add) {
        return Assembly::BinaryOperator::Add;
    }

    if (binary_operator == Tacky::BinaryOperator::Subtract) {
        return Assembly::BinaryOperator::Sub;
    }

    if (binary_operator == Tacky::BinaryOperator::Multiply) {
        return Assembly::BinaryOperator::Mult;
    }

    throw std::runtime_error("Incorrect Tacky Binops");
    
}

Assembly::UnaryOperator AssemblyGenerator::generate_unop(
    Tacky::UnaryOperator unary_operator
) {
    if (unary_operator == Tacky::UnaryOperator::Complement) {
        return Assembly::UnaryOperator::Not;
    }

    if (unary_operator == Tacky::UnaryOperator::Negate) {
        return Assembly::UnaryOperator::Neg;
    }

    throw std::runtime_error(
        "Unsupported TACKY unary operator in assembly generation"
    );
}

int AssemblyGenerator::replace_pseudos(
    std::vector<std::unique_ptr<Assembly::Instruction>>& instructions
) {
    std::unordered_map<std::string, int> pseudo_offsets;
    int current_offset = 0;

    auto replace_operand = [&](std::unique_ptr<Assembly::Operand>& operand) {
        const Assembly::PseudoOperand* pseudo_operand =
            dynamic_cast<const Assembly::PseudoOperand*>(operand.get());

        if (pseudo_operand == nullptr) {
            return;
        }

        const std::string identifier = pseudo_operand->identifier;
        auto existing_offset = pseudo_offsets.find(identifier);
        int stack_offset;

        if (existing_offset == pseudo_offsets.end()) {
            current_offset -= 4;
            stack_offset = current_offset;
            pseudo_offsets.emplace(identifier, stack_offset);
        } else {
            stack_offset = existing_offset->second;
        }

        operand = std::make_unique<Assembly::StackOperand>(stack_offset);
    };

    for (std::unique_ptr<Assembly::Instruction>& instruction : instructions) {
        Assembly::MovInstruction* mov_instruction =
            dynamic_cast<Assembly::MovInstruction*>(instruction.get());

        if (mov_instruction != nullptr) {
            replace_operand(mov_instruction->src);
            replace_operand(mov_instruction->dst);
            continue;
        }

        Assembly::UnaryInstruction* unary_instruction =
            dynamic_cast<Assembly::UnaryInstruction*>(instruction.get());

        if (unary_instruction != nullptr) {
            replace_operand(unary_instruction->operand);
            continue;
        }

        Assembly::BinaryInstruction* binary_instruction =
            dynamic_cast<Assembly::BinaryInstruction*>(instruction.get());

        if (binary_instruction != nullptr) {
            replace_operand(binary_instruction->operand1);
            replace_operand(binary_instruction->operand2);
        }
    }

    return -current_offset;
}

std::vector<std::unique_ptr<Assembly::Instruction>>
AssemblyGenerator::fix_instructions(
    std::vector<std::unique_ptr<Assembly::Instruction>> instructions
) {
    std::vector<std::unique_ptr<Assembly::Instruction>> fixed_instructions;

    for (std::unique_ptr<Assembly::Instruction>& instruction : instructions) {
        Assembly::MovInstruction* mov_instruction =
            dynamic_cast<Assembly::MovInstruction*>(instruction.get());

        const bool source_is_stack =
            mov_instruction != nullptr &&
            dynamic_cast<Assembly::StackOperand*>(mov_instruction->src.get()) != nullptr;

        const bool destination_is_stack =
            mov_instruction != nullptr &&
            dynamic_cast<Assembly::StackOperand*>(mov_instruction->dst.get()) != nullptr;

        if (source_is_stack && destination_is_stack) {
            fixed_instructions.push_back(
                std::make_unique<Assembly::MovInstruction>(
                    std::move(mov_instruction->src),
                    std::make_unique<Assembly::RegisterOperand>(Assembly::Register::R10)
                )
            );

            fixed_instructions.push_back(
                std::make_unique<Assembly::MovInstruction>(
                    std::make_unique<Assembly::RegisterOperand>(Assembly::Register::R10),
                    std::move(mov_instruction->dst)
                )
            );
            continue;
        }

        Assembly::Idiv* idiv = dynamic_cast<Assembly::Idiv*>(instruction.get());
        const bool src_is_imm =
            idiv != nullptr &&
            dynamic_cast<Assembly::ImmOperand*>(idiv->operand.get()) != nullptr;

        if (src_is_imm) {
            fixed_instructions.push_back(
                std::make_unique<Assembly::MovInstruction>(
                    std::move(idiv->operand),
                    std::make_unique<Assembly::RegisterOperand>(Assembly::Register::R10)
                )
            );

            fixed_instructions.push_back(
                std::make_unique<Assembly::Idiv>(
                    std::make_unique<Assembly::RegisterOperand>(Assembly::Register::R10)
                )
            );
            continue;
        }

        Assembly::BinaryInstruction* binary_instruction =
            dynamic_cast<Assembly::BinaryInstruction*>(instruction.get());

        if (binary_instruction != nullptr) {
            const bool binary_source_is_stack =
                dynamic_cast<Assembly::StackOperand*>(
                    binary_instruction->operand1.get()
                ) != nullptr;

            Assembly::StackOperand* binary_destination_stack =
                dynamic_cast<Assembly::StackOperand*>(
                    binary_instruction->operand2.get()
                );

            const bool is_add_or_subtract =
                binary_instruction->binary_operator == Assembly::BinaryOperator::Add ||
                binary_instruction->binary_operator == Assembly::BinaryOperator::Sub;

            if (is_add_or_subtract &&
                binary_source_is_stack &&
                binary_destination_stack != nullptr) {
                Assembly::BinaryOperator binary_operator =
                    binary_instruction->binary_operator;

                fixed_instructions.push_back(
                    std::make_unique<Assembly::MovInstruction>(
                        std::move(binary_instruction->operand1),
                        std::make_unique<Assembly::RegisterOperand>(Assembly::Register::R10)
                    )
                );

                fixed_instructions.push_back(
                    std::make_unique<Assembly::BinaryInstruction>(
                        binary_operator,
                        std::make_unique<Assembly::RegisterOperand>(Assembly::Register::R10),
                        std::move(binary_instruction->operand2)
                    )
                );
                continue;
            }

            if (binary_instruction->binary_operator == Assembly::BinaryOperator::Mult &&
                binary_destination_stack != nullptr) {
                const int destination_offset = binary_destination_stack->offset;

                fixed_instructions.push_back(
                    std::make_unique<Assembly::MovInstruction>(
                        std::move(binary_instruction->operand2),
                        std::make_unique<Assembly::RegisterOperand>(Assembly::Register::R11)
                    )
                );

                fixed_instructions.push_back(
                    std::make_unique<Assembly::BinaryInstruction>(
                        Assembly::BinaryOperator::Mult,
                        std::move(binary_instruction->operand1),
                        std::make_unique<Assembly::RegisterOperand>(Assembly::Register::R11)
                    )
                );

                fixed_instructions.push_back(
                    std::make_unique<Assembly::MovInstruction>(
                        std::make_unique<Assembly::RegisterOperand>(Assembly::Register::R11),
                        std::make_unique<Assembly::StackOperand>(destination_offset)
                    )
                );
                continue;
            }
        }

        fixed_instructions.push_back(std::move(instruction));
    }

    return fixed_instructions;
}


/*  */

std::string AssemblyEmitter::emit_program(const Assembly::Program& program) {
    std::string output = emit_function(program.function);

#ifdef __linux__
    output += ".section .note.GNU-stack,\"\",@progbits\n";
#endif

    return output;
}

std::string AssemblyEmitter::emit_function(const Assembly::FunctionDefinition& function) {
    std::string output;

    output += ".globl " + function.name + "\n";
    output += function.name + ":\n";
    output += "    pushq %rbp\n";
    output += "    movq %rsp, %rbp\n";

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

    const Assembly::UnaryInstruction* unary_instruction =
        dynamic_cast<const Assembly::UnaryInstruction*>(&instruction);

    if (unary_instruction != nullptr) {
        std::string operand = emit_operand(*unary_instruction->operand);

        if (unary_instruction->unary_operator == Assembly::UnaryOperator::Neg) {
            return "    negl " + operand + "\n";
        }

        if (unary_instruction->unary_operator == Assembly::UnaryOperator::Not) {
            return "    notl " + operand + "\n";
        }
    }

    const Assembly::AllocateStackInstruction* allocate_stack_instruction =
        dynamic_cast<const Assembly::AllocateStackInstruction*>(&instruction);

    if (allocate_stack_instruction != nullptr) {
        return "    subq $" + std::to_string(allocate_stack_instruction->bytes) +
               ", %rsp\n";
    }

    const Assembly::RetInstruction* ret_instruction = dynamic_cast<const Assembly::RetInstruction*>(&instruction);

    if (ret_instruction != nullptr) {
        return "    movq %rbp, %rsp\n"
               "    popq %rbp\n"
               "    ret\n";
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
        if (register_operand->reg == Assembly::Register::AX) {
            return "%eax";
        }

        if (register_operand->reg == Assembly::Register::R10) {
            return "%r10d";
        }
    }

    const Assembly::PseudoOperand* pseudo_operand =
        dynamic_cast<const Assembly::PseudoOperand*>(&operand);

    if (pseudo_operand != nullptr) {
        throw std::runtime_error(
            "Pseudo operand must be replaced before assembly emission: " +
            pseudo_operand->identifier
        );
    }

    const Assembly::StackOperand* stack_operand =
        dynamic_cast<const Assembly::StackOperand*>(&operand);

    if (stack_operand != nullptr) {
        return std::to_string(stack_operand->offset) + "(%rbp)";
    }

    throw std::runtime_error("Unsupported operand in assembly emitter");
}
