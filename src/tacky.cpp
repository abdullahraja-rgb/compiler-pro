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
    std::vector<std::unique_ptr<Tacky::Instruction>> inst_list = generate_instructions(*func_def.body);
    return Tacky::FunctionDefinition(std::move(func_name), std::move(inst_list));
}

std::vector<std::unique_ptr<Tacky::Instruction>>
TackyGenerator::generate_instructions(const Statement& statement) {
    std::vector<std::unique_ptr<Tacky::Instruction>> instructions;

    const ReturnStatement* return_statement =
        dynamic_cast<const ReturnStatement*>(&statement);

    if (return_statement != nullptr) {
        std::unique_ptr<Tacky::Value> return_value = generate_value(
            *return_statement->value,
            instructions
        );

        instructions.push_back(
            std::make_unique<Tacky::ReturnInstruction>(
                std::move(return_value)
            )
        );

        return instructions;
    }

    throw std::runtime_error(
        "Unsupported statement in TACKY generation"
    );
}

std::unique_ptr<Tacky::Value> TackyGenerator::generate_value(
    const Expression& expression,
    std::vector<std::unique_ptr<Tacky::Instruction>>& instructions
) {
    const ConstantExpression* constant_expression =
        dynamic_cast<const ConstantExpression*>(&expression);

    if (constant_expression != nullptr) {
        return std::make_unique<Tacky::ConstantValue>(
            constant_expression->value
        );
    }

    const UnaryExpression* unary_expression =
        dynamic_cast<const UnaryExpression*>(&expression);

    if (unary_expression != nullptr) {
        std::unique_ptr<Tacky::Value> source = generate_value(
            *unary_expression->value,
            instructions
        );

        std::string destination_name = make_temporary();
        Tacky::UnaryOperator tacky_operator =
            generate_unop(unary_expression->unary_operator);

        instructions.push_back(
            std::make_unique<Tacky::UnaryInstruction>(
                tacky_operator,
                std::move(source),
                std::make_unique<Tacky::VariableValue>(destination_name)
            )
        );

        return std::make_unique<Tacky::VariableValue>(
            destination_name
        );
    }

    throw std::runtime_error(
        "Unsupported expression in TACKY generation"
    );
}

Tacky::UnaryOperator TackyGenerator::generate_unop(
    ::UnaryOperator ast_operator
) {
    if (ast_operator == ::Complement) {
        return Tacky::UnaryOperator::Complement;
    } else if (ast_operator == ::Negate) {
        return Tacky::UnaryOperator::Negate;
    }

    throw std::runtime_error(
        "Unsupported unary operator in TACKY generation"
    );
}

std::string TackyGenerator::make_temporary() {
    return "tmp." + std::to_string(temporary_counter++);
}
