#pragma once

#include "ast.hpp"

#include <string>
#include <unordered_map>

struct SemanticAnalyzer {
    void resolve_program(Program& program);

private:
    using VariableMap = std::unordered_map<std::string, std::string>;

    void resolve_function(FunctionDefinition& function);
    void resolve_block_item(BlockItem& block_item, VariableMap& variable_map);
    void resolve_declaration(Declaration& declaration, VariableMap& variable_map);
    void resolve_statement(Statement& statement, VariableMap& variable_map);
    void resolve_expression(Expression& expression, VariableMap& variable_map);
    std::string make_unique_name(const std::string& original_name);

    int variable_counter = 0;
};
