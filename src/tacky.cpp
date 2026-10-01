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

    const BinaryExpression* bin_exp = dynamic_cast<const BinaryExpression*>(&expression);
    /* check if the binary operator is and or not if it is create  */

    if (bin_exp != nullptr) {
        // e.g BinExp(And, e1, e2)
        if (bin_exp->binary_operator == BinaryOperator::And) {
            return generate_logical_and(*bin_exp, instructions);
            
        }
        if (bin_exp->binary_operator == BinaryOperator::Or) {
            return generate_logical_or(*bin_exp, instructions);
            
        }

        std::unique_ptr<Tacky::Value> source1 = generate_value(
            *bin_exp->exp1,
            instructions
        );
        std::unique_ptr<Tacky::Value> source2 = generate_value(
            *bin_exp->exp2,
            instructions
        );


        std::string dst_name = make_temporary();
        Tacky::BinaryOperator binary_op = generate_binop(
            bin_exp->binary_operator
        );
        instructions.push_back(std::make_unique<Tacky::BinaryInstruction>(
            binary_op, 
            std::move(source1),
            std::move(source2),
            std::make_unique<Tacky::VariableValue>(dst_name)
        ));
        return std::make_unique<Tacky::VariableValue>(dst_name);

    }
    throw std::runtime_error(
        "Unsupported expression in TACKY generation"
    );
}

std::unique_ptr<Tacky::Value> TackyGenerator::generate_logical_and(
    const BinaryExpression& expression,
    std::vector<std::unique_ptr<Tacky::Instruction>>& instructions
) {
    std::string false_label = "and_false." + make_temporary();
    std::string end_label = "and_end." + make_temporary();
    std::string destination_name = make_temporary();

    // generate_value for e1 -> might be a const val or an expression again (unary or binary)
    // it will resolve to 1 or 0 or a constant
    std::unique_ptr<Tacky::Value> left = generate_value(
        *expression.exp1,
        instructions
    );
    // if zero then go to false label
    instructions.push_back(
        std::make_unique<Tacky::JumpIfZero>(
            std::move(left),
            false_label
        )
    );

    std::unique_ptr<Tacky::Value> right = generate_value(
        *expression.exp2,
        instructions
    );
    instructions.push_back(
        std::make_unique<Tacky::JumpIfZero>(
            std::move(right),
            false_label
        )
    );

    instructions.push_back(
        std::make_unique<Tacky::Copy>(
            std::make_unique<Tacky::ConstantValue>(1),
            std::make_unique<Tacky::VariableValue>(destination_name)
        )
    );
    instructions.push_back(
        std::make_unique<Tacky::Jump>(end_label)
    );
    instructions.push_back(
        std::make_unique<Tacky::Label>(false_label)
    );
    instructions.push_back(
        std::make_unique<Tacky::Copy>(
            std::make_unique<Tacky::ConstantValue>(0),
            std::make_unique<Tacky::VariableValue>(destination_name)
        )
    );
    instructions.push_back(
        std::make_unique<Tacky::Label>(end_label)
    );

    return std::make_unique<Tacky::VariableValue>(destination_name);
}

std::unique_ptr<Tacky::Value> TackyGenerator::generate_logical_or(
    const BinaryExpression& expression,
    std::vector<std::unique_ptr<Tacky::Instruction>>& instructions
) {
    std::string true_label = "or_true." + make_temporary();
    std::string end_label = "or_end." + make_temporary();
    std::string destination_name = make_temporary();

    std::unique_ptr<Tacky::Value> left = generate_value(
        *expression.exp1,
        instructions
    );
    instructions.push_back(
        std::make_unique<Tacky::JumpIfNotZero>(
            std::move(left),
            true_label
        )
    );

    std::unique_ptr<Tacky::Value> right = generate_value(
        *expression.exp2,
        instructions
    );
    instructions.push_back(
        std::make_unique<Tacky::JumpIfNotZero>(
            std::move(right),
            true_label
        )
    );

    instructions.push_back(
        std::make_unique<Tacky::Copy>(
            std::make_unique<Tacky::ConstantValue>(0),
            std::make_unique<Tacky::VariableValue>(destination_name)
        )
    );
    instructions.push_back(
        std::make_unique<Tacky::Jump>(end_label)
    );
    instructions.push_back(
        std::make_unique<Tacky::Label>(true_label)
    );
    instructions.push_back(
        std::make_unique<Tacky::Copy>(
            std::make_unique<Tacky::ConstantValue>(1),
            std::make_unique<Tacky::VariableValue>(destination_name)
        )
    );
    instructions.push_back(
        std::make_unique<Tacky::Label>(end_label)
    );

    return std::make_unique<Tacky::VariableValue>(destination_name);
}

Tacky::UnaryOperator TackyGenerator::generate_unop(
    ::UnaryOperator ast_operator
) {
    if (ast_operator == ::Complement) {
        return Tacky::UnaryOperator::Complement;
    } else if (ast_operator == ::Negate) {
        return Tacky::UnaryOperator::Negate;
    } else {
        return Tacky::UnaryOperator::Not;
    }

    throw std::runtime_error(
        "Unsupported unary operator in TACKY generation"
    );
}

Tacky::BinaryOperator TackyGenerator::generate_binop(
    ::BinaryOperator ast_bi_operator
) {
    if (ast_bi_operator == ::Add) {
        return Tacky::BinaryOperator::Add;
    }
    if (ast_bi_operator == ::Subtract) {
        return Tacky::BinaryOperator::Subtract;
    }
    if (ast_bi_operator == ::Remainder) {
        return Tacky::BinaryOperator::Remainder;
    }
    if (ast_bi_operator == ::Multiply) {
        return Tacky::BinaryOperator::Multiply;
    }
    if (ast_bi_operator == ::Divide) {
        return Tacky::BinaryOperator::Divide;
    }
    if (ast_bi_operator == ::Equal) {
        return Tacky::BinaryOperator::Equal;
    }
    if (ast_bi_operator == ::NotEqual) {
        return Tacky::BinaryOperator::NotEqual;
    }
    if (ast_bi_operator == ::LessThan) {
        return Tacky::BinaryOperator::LessThan;
    }
    if (ast_bi_operator == ::LessOrEqual) {
        return Tacky::BinaryOperator::LessOrEqual;
    }
    if (ast_bi_operator == ::GreaterThan) {
        return Tacky::BinaryOperator::GreaterThan;
    }
    if (ast_bi_operator == ::GreaterOrEqual) {
        return Tacky::BinaryOperator::GreaterOrEqual;
    }
    throw std::runtime_error("Wrong Ast Binary Operator");
}

std::string TackyGenerator::make_temporary() {
    return "tmp." + std::to_string(temporary_counter++);
}
