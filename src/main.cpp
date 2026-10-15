// program summary: reads a c files content
#include <iostream>
#include <iomanip>
#include <string>
#include <filesystem>
// file handling 
#include <fstream>
// string stream - storing file contents into string
#include <sstream>
#include "../include/mycc/lexer.hpp"
// #include "lexer.cpp"
// #include "parser.cpp"
// #include "ast.cpp"
#include "../include/mycc/ast.hpp"
#include "../include/mycc/parser.hpp"
#include "../include/mycc/codegen.hpp"
#include "../include/mycc/tacky.hpp"
#include "../include/mycc/semantic_analysis.hpp"
#include <unordered_set>
#include <cstdlib>
#include <chrono>


/* if arg ==3 check for options
if argc == 2 execute normal flow
if argc <= 1 no input if arc > 3 more input */

std::string unary_operator_name(UnaryOperator op) {
    if (op == UnaryOperator::Complement) {
        return "Complement";
    }
    if (op == UnaryOperator::Not) {
        return "LogicalNot";
    }

    return "Negate";
}

std::string binary_operator_name(BinaryOperator op) {
    switch (op) {
        case BinaryOperator::Add:
            return "Add";
        case BinaryOperator::Subtract:
            return "Subtract";
        case BinaryOperator::Multiply:
            return "Multiply";
        case BinaryOperator::Divide:
            return "Divide";
        case BinaryOperator::Remainder:
            return "Remainder";
        case BinaryOperator::And:
            return "And";
        case BinaryOperator::Or:
            return "Or";
        case BinaryOperator::Equal:
            return "Equal";
        case BinaryOperator::NotEqual:
            return "NotEqual";
        case BinaryOperator::LessThan:
            return "LessThan";
        case BinaryOperator::LessOrEqual:
            return "LessOrEqual";
        case BinaryOperator::GreaterThan:
            return "GreaterThan";
        case BinaryOperator::GreaterOrEqual:
            return "GreaterOrEqual";
    }

    throw std::runtime_error("Incorrect binop");
}

void print_expression(const Expression& expression, int indent) {
    std::string spaces(indent, ' ');

    if (const auto* constant = dynamic_cast<const ConstantExpression*>(&expression)) {
        std::cout << spaces
                  << "ConstantExpression value="
                  << constant->value
                  << '\n';
    } else if (const auto* variable = dynamic_cast<const VariableExpression*>(&expression)) {
        std::cout << spaces
                  << "VariableExpression name="
                  << variable->name
                  << '\n';
    } else if (const auto* unary = dynamic_cast<const UnaryExpression*>(&expression)) {
        std::cout << spaces
                  << "UnaryExpression operator="
                  << unary_operator_name(unary->unary_operator)
                  << '\n';

        print_expression(*unary->value, indent + 2);
    } else if (const auto* binary = dynamic_cast<const BinaryExpression*>(&expression)) {
        std::cout << spaces
                  << "BinaryExpression operator="
                  << binary_operator_name(binary->binary_operator)
                  << '\n';

        print_expression(*binary->exp1, indent + 2);
        print_expression(*binary->exp2, indent + 2);
    } else if (const auto* assignment =
                   dynamic_cast<const AssignmentExpression*>(&expression)) {
        std::cout << spaces << "AssignmentExpression\n";
        print_expression(*assignment->left, indent + 2);
        print_expression(*assignment->right, indent + 2);
    }
}


int main(int argc, char* argv[]) {
    std::string filename;
    // initially an empty string
    std::string option;
    const std::unordered_set<std::string> valid_options{"--lex", "--parse", "--validate", "--codegen", "--tacky", "--benchmark"};

    if (argc > 3 || argc < 2) {
        std::cout << "Invalid input only enter C Filename" << std::endl;
        return 1;
    } else if (argc == 2) {
        filename = argv[1];
    } else if (argc == 3) {
        option = argv[1];
        filename = argv[2];

        if (!valid_options.contains(option)) {
            std::cerr << "The specified operation isnt supported" << std::endl;
            return -1;
        }
    }
    
    bool benchmark_mode = option == "--benchmark";

    long long preprocessing_time = 0;
    long long lexing_time = 0;
    long long parsing_time = 0;
    long long tacky_time = 0;
    long long assembly_generation_time = 0;
    long long code_emission_time = 0;

    auto total_start = std::chrono::steady_clock::now();
    

    // checks if a c file
    if (filename.ends_with(".c")) {

        std::filesystem::path preprocessed_path(filename);
        preprocessed_path.replace_extension(".i");
        std::string command =
            "gcc -E -P \"" + filename +
            "\" -o \"" + preprocessed_path.string() + "\"";

        // start preprocessing
        auto preprocessing_start = std::chrono::steady_clock::now();
        if (std::system(command.c_str()) != 0) {
            std::cerr << "Preprocessing failed\n";
            return 1;
        }
        auto preprocessing_end = std::chrono::steady_clock::now();
        preprocessing_time =
            std::chrono::duration_cast<std::chrono::microseconds>(
                preprocessing_end - preprocessing_start
            ).count();

        // read the file content
        std::ifstream file(preprocessed_path);
        if (!file) {
            std::cout << "Below is not a file" << std::endl;
            return 1;
        }
        if (!file.is_open()) {
            std::cerr << "Couldnt open the file" << std::endl;
            return 1;
        }
       
        // std::string filewords; need to create a sstring for this
        // filewords << file.rdbuf(); doesnt work type mismatch
        std::stringstream filewords;
        filewords << file.rdbuf();
        // filewords = filewords.str() cant store sstring type to a string
        std::string filewords_str = filewords.str();

        auto lexing_start = std::chrono::steady_clock::now();
        // we need to call the tokenise function we declared in the lexer.hpp
        std::vector<Token> filewords_token_vector = tokenise(filewords_str);
        auto lexing_end = std::chrono::steady_clock::now();
        lexing_time =
            std::chrono::duration_cast<std::chrono::microseconds>(
                lexing_end - lexing_start
            ).count();

        if (!benchmark_mode) {
            // conventional method to print a vector of structs
            for (const Token& obj : filewords_token_vector) {
                std::cout << obj.type << " " << obj.value << std::endl;
            }

        }


        if (option == "--lex") {
            return 0;
        }

        // create a parser object
        Parser parser(filewords_token_vector);
        auto parsing_start = std::chrono::steady_clock::now();
        Program ast_rep = parser.parse_program();
        auto parsing_end = std::chrono::steady_clock::now();
        parsing_time =
            std::chrono::duration_cast<std::chrono::microseconds>(
                parsing_end - parsing_start
            ).count();


        if (!benchmark_mode) {
            std::cout << "AST Incoming ..." << std::endl;
            std::cout << "" << std::endl;
            // ------------- pretty-print the tree ------------------------------
            std::cout << "Program\n";
            std::cout << "  FunctionDefinition name=" << ast_rep.function.name << '\n';

            for (const std::unique_ptr<BlockItem>& block_item : ast_rep.function.body) {
                if (const auto* declaration_item =
                        dynamic_cast<const DeclarationBlockItem*>(block_item.get())) {
                    std::cout << "    Declaration name="
                              << declaration_item->declaration.name
                              << '\n';
                    if (declaration_item->declaration.initializer != nullptr) {
                        print_expression(
                            *declaration_item->declaration.initializer,
                            6
                        );
                    }
                    continue;
                }

                const auto* statement_item =
                    dynamic_cast<const StatementBlockItem*>(block_item.get());

                if (statement_item == nullptr) {
                    continue;
                }

                if (const auto* return_statement = dynamic_cast<const ReturnStatement*>(
                        statement_item->statement.get())) {
                    std::cout << "    ReturnStatement\n";
                    print_expression(*return_statement->value, 6);
                } else if (const auto* expression_statement =
                               dynamic_cast<const ExpressionStatement*>(
                                   statement_item->statement.get())) {
                    std::cout << "    ExpressionStatement\n";
                    print_expression(*expression_statement->expression, 6);
                } else if (dynamic_cast<const NullStatement*>(
                               statement_item->statement.get()) != nullptr) {
                    std::cout << "    NullStatement\n";
                }
            }
            // ------------------------------------------------------------
        }


        if (option == "--parse") {
            return 0;
        }

        SemanticAnalyzer semantic_analyzer;
        semantic_analyzer.resolve_program(ast_rep);

        if (option == "--validate") {
            return 0;
        }





        // tacky generation
        TackyGenerator tacky_generator;
        auto ir_start = std::chrono::steady_clock::now();
        Tacky::Program tacky_program = tacky_generator.generate_program(ast_rep);
        auto ir_end = std::chrono::steady_clock::now();
        tacky_time =
            std::chrono::duration_cast<std::chrono::microseconds>(
                ir_end - ir_start
            ).count();




        if (option == "--tacky") {
            return 0;
        }

        // call the assembly generator
        AssemblyGenerator assembly_generator;
        auto assembly_generation_start = std::chrono::steady_clock::now();
        Assembly::Program assembly_program = assembly_generator.generate_program(tacky_program);
        auto assembly_generation_end = std::chrono::steady_clock::now();
        assembly_generation_time =
            std::chrono::duration_cast<std::chrono::microseconds>(
                assembly_generation_end - assembly_generation_start
            ).count();

        if (option == "--codegen") {
            return 0;
        }

        // wiring the assembly text generator
        auto code_emission_start = std::chrono::steady_clock::now();
        AssemblyEmitter assembly_emitter;
        std::string assembly_text = assembly_emitter.emit_program(assembly_program);
        std::filesystem::path assembly_path(filename);
        assembly_path.replace_extension(".s");
        std::ofstream assembly_file(assembly_path);
        if (!assembly_file) {
            std::cerr << "Could not create assembly file" << std::endl;
            return 1;
        }
        assembly_file << assembly_text;
        if (!assembly_file) {
            std::cerr << "Could not write to the assembly file" << std::endl;
            return 1;
        }
        assembly_file.close();
        auto code_emission_end = std::chrono::steady_clock::now();
        code_emission_time =
            std::chrono::duration_cast<std::chrono::microseconds>(
                code_emission_end - code_emission_start
            ).count();

        if (benchmark_mode) {
            auto total_end = std::chrono::steady_clock::now();
            long long total_time =
                std::chrono::duration_cast<std::chrono::microseconds>(
                    total_end - total_start
                ).count();

            // stdout the benchmarking data
            std::cout << "BENCHMARK,"
                      << preprocessing_time << ","
                      << lexing_time << ","
                      << parsing_time << ","
                      << tacky_time << ","
                      << assembly_generation_time << ","
                      << code_emission_time << ","
                      << total_time << std::endl;
        }


        // now we have stored the string in filewords we now need to pass this to our lexer
    } else {
        std::cout << "Its not a c file. Enter a C Filename: " << std::endl;
        return 1;
    }

    return 0;
}
