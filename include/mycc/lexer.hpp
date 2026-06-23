// Placeholder for the lexer interface.
// The lexer will eventually turn raw source text into a stream of tokens.

// this is like all the important declarations for lexer.cpp file pragman once iensures that 
// the same types or functions arent defined multiple times to prevent any errors
#pragma once

#include <string>
#include <vector>

struct Token {
    std::string type;
    std::string value;
};

// function declaration which will be defined in lexer.cpp
// argument is a const because we dont want the lexer to modify the rep for file contents
std::vector<Token> tokenise(const std::string& filewords_str);