/*  
program = Program(function_definition)
function_definition = Function(identifier name, statement body)
statement = Return(exp)
exp = Constant(int) | Unary(unary_operator, exp)
unary_operator = Complement | Negate

<program> ::= <function>
<function> ::= "int" <identifier> "(" "void" ")" "{" <statement> "}"
<statement> ::= "return" <exp> ";"
<exp> ::= <int> | <unop> <exp> | "(" <exp> ")"
<unop> ::= "-" | "~"
<identifier> ::= ? An identifier token ?
<int> ::= ? A constant token ?



program = Program(function_definition)
function_definition = Function(identifier, instruction* body)
instruction = Return(val) | Unary(unary_operator, val src, val dst)
val = Constant(int) | Var(identifier)
unary_operator = Complement | Negate


*/

#include <utility>
#include <stdexcept>
#include "../include/mycc/tacky.hpp"
#include "../include/mycc/ast.hpp"
#include <memory>
#include <string>
#include <vector>

Tacky::Program TackyGenerator::generate_program(const Program& program) {
    Tacky::FunctionDefinition func_generated = generate_function(program.function);
    return Tacky::Program(std::move(func_generated));

}

Tacky::FunctionDefinition TackyGenerator::generate_function(const FunctionDefinition& func_def) {
    std::string func_name = func_def.name;
    std::vector<std::unique_ptr<Tacky::Instruction>> inst_list = generate_instruction(*func_def.body);
    return Tacky::FunctionDefinition(std::move(func_name), std::move(inst_list));
}

std::vector<std::unique_ptr<Tacky::Instruction>> TackyGenerator::generate_instruction(const Statement& statement) {
    std::vector<std::unique_ptr<Tacky::Instruction>> instructions;
    
    // check if the input statement is a return via dynamic casting
    const ReturnStatement* rcv_statement = dynamic_cast<const ReturnStatement*>(&statement);
    // if casting didnt fail then its a return statement
    if (rcv_statement != nullptr) {
        // move the srcc=expression to dest which defs to eax
        /* access the value  */
        const Expression& expression = *rcv_statement->value;
        std::vector<std::unique_ptr<Tacky::Instruction>> express_val = generate_value(expression);

        for (std::unique_ptr<Tacky::Instruction>& instruction : express_val) {
            instructions.push_back(std::move(instruction));
        }

        instructions.push_back(std::make_unique<Assembly::RetInstruction>());

        return instructions;
    }

    throw std::runtime_error("Unsupported statement in code generation");
}

std::unique_ptr<Tacky::Value> generate_value(
    const Expression& expression,
    std::vector<std::unique_ptr<Tacky::Instruction>>& instructions
) {

}
