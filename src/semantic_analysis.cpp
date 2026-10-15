#include "../include/mycc/semantic_analysis.hpp"

#include <stdexcept>

void SemanticAnalyzer::resolve_program(Program& program) {
    resolve_function(program.function);
}

void SemanticAnalyzer::resolve_function(FunctionDefinition& function) {
    VariableMap variable_map;

    for (std::unique_ptr<BlockItem>& block_item : function.body) {
        resolve_block_item(*block_item, variable_map);
    }
}

void SemanticAnalyzer::resolve_block_item(
    BlockItem& block_item,
    VariableMap& variable_map
) {
    if (auto* declaration_item =
            dynamic_cast<DeclarationBlockItem*>(&block_item)) {
        resolve_declaration(declaration_item->declaration, variable_map);
        return;
    }

    if (auto* statement_item = dynamic_cast<StatementBlockItem*>(&block_item)) {
        resolve_statement(*statement_item->statement, variable_map);
        return;
    }

    throw std::runtime_error("Semantic error: unsupported block item");
}

void SemanticAnalyzer::resolve_declaration(
    Declaration& declaration,
    VariableMap& variable_map
) {
    if (variable_map.contains(declaration.name)) {
        throw std::runtime_error(
            "Semantic error: duplicate variable declaration '" +
            declaration.name + "'"
        );
    }

    std::string original_name = declaration.name;
    std::string unique_name = make_unique_name(original_name);
    variable_map.emplace(original_name, unique_name);
    declaration.name = unique_name;

    if (declaration.initializer != nullptr) {
        resolve_expression(*declaration.initializer, variable_map);
    }
}

void SemanticAnalyzer::resolve_statement(
    Statement& statement,
    VariableMap& variable_map
) {
    if (auto* return_statement = dynamic_cast<ReturnStatement*>(&statement)) {
        resolve_expression(*return_statement->value, variable_map);
        return;
    }

    if (auto* expression_statement =
            dynamic_cast<ExpressionStatement*>(&statement)) {
        resolve_expression(*expression_statement->expression, variable_map);
        return;
    }

    if (dynamic_cast<NullStatement*>(&statement) != nullptr) {
        return;
    }

    throw std::runtime_error("Semantic error: unsupported statement");
}

void SemanticAnalyzer::resolve_expression(
    Expression& expression,
    VariableMap& variable_map
) {
    if (dynamic_cast<ConstantExpression*>(&expression) != nullptr) {
        return;
    }

    if (auto* variable = dynamic_cast<VariableExpression*>(&expression)) {
        auto resolved_name = variable_map.find(variable->name);

        if (resolved_name == variable_map.end()) {
            throw std::runtime_error(
                "Semantic error: undeclared variable '" + variable->name + "'"
            );
        }

        variable->name = resolved_name->second;
        return;
    }

    if (auto* unary = dynamic_cast<UnaryExpression*>(&expression)) {
        resolve_expression(*unary->value, variable_map);
        return;
    }

    if (auto* binary = dynamic_cast<BinaryExpression*>(&expression)) {
        resolve_expression(*binary->exp1, variable_map);
        resolve_expression(*binary->exp2, variable_map);
        return;
    }

    if (auto* assignment = dynamic_cast<AssignmentExpression*>(&expression)) {
        if (dynamic_cast<VariableExpression*>(assignment->left.get()) == nullptr) {
            throw std::runtime_error(
                "Semantic error: assignment target is not a variable"
            );
        }

        resolve_expression(*assignment->left, variable_map);
        resolve_expression(*assignment->right, variable_map);
        return;
    }

    throw std::runtime_error("Semantic error: unsupported expression");
}

std::string SemanticAnalyzer::make_unique_name(
    const std::string& original_name
) {
    return "var." + original_name + "." +
        std::to_string(variable_counter++);
}
