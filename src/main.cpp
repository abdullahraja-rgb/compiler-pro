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
#include <unordered_set>


/* if arg ==3 check for options
if argc == 2 execute normal flow
if argc <= 1 no input if arc > 3 more input */


int main(int argc, char* argv[]) {
    std::string filename;
    std::string option;
    const std::unordered_set<std::string> valid_options{"--lex", "--parse", "--codegen"};

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

    // checks if a c file
    if (filename.ends_with(".c")) {
        // read the file content
        std::ifstream file(filename);
        if (!file) {
            std::cout << "Below is not a file" << std::endl;
            return 1;
        }
        // std::string filewords; need to create a sstring for this
        // filewords << file.rdbuf(); doesnt work type mismatch
        std::stringstream filewords;
        filewords << file.rdbuf();
        // filewords = filewords.str() cant store sstring type to a string
        std::string filewords_str = filewords.str();
        // we need to call the tokenise function we declared in the lexer.hpp
        std::vector<Token> filewords_token_vector = tokenise(filewords_str);
        // conventional method to print a vector of structs
        for (const Token& obj : filewords_token_vector) {
            std::cout << obj.type << " " << obj.value << std::endl;
        }

        if (option == "--lex") {
            return 0;
        }

        // create a parser object
        Parser parser(filewords_token_vector);
        Program ast_rep = parser.parse_program();
        std::cout << "AST Incoming ..." << std::endl;
        std::cout << "" << std::endl;
        // ------------- pretty-print the tree ------------------------------
        std::cout << "Program" << std::endl;
        std::cout << "  FunctionDefinition name=" << ast_rep.function.name << std::endl;

        if (ReturnStatement* return_statement = dynamic_cast<ReturnStatement*>(ast_rep.function.body.get())) {
            std::cout << "    ReturnStatement" << std::endl;

            if (ConstantExpression* constant_expression = dynamic_cast<ConstantExpression*>(return_statement->value.get())) {
                std::cout << "      ConstantExpression value=" << constant_expression->value << std::endl;
            }
        }
        // ------------------------------------------------------------

        if (option == "--parse") {
            return 0;
        }

        // call the assembly generator
        AssemblyGenerator assembly_generator;
        Assembly::Program assembly_program = assembly_generator.generate_program(ast_rep);

        if (option == "--codegen") {
            return 0;
        }

        // now we have stored the string in filewords we now need to pass this to our lexer
    } else {
        std::cout << "Its not a c file. Enter a C Filename: " << std::endl;
        return 1;
    }

    return 0;
}
