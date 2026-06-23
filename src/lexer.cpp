// Placeholder for lexer implementation.
// First goal later: recognize `int main(void) { return 42; }`.

/* first we open the file we want to work on and then we access the stream of words/chars
    create tokens and then some way of a small storage of all token possibly a list/arr
    
    use of regex to determine the token patterns
    
    questions:
    do we need to have the file opening logic on lexer?? 
    how do we store the tokens would an array work?
    do we use a hashtable for the regex patterns for tokens??
    */

#include "../include/mycc/lexer.hpp"
#include <cctype>
#include <cstddef>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

struct tokenPatterns {
    std::string type;
    std::regex regexPattern;
};

// preferred over a hashmap due to its ordering property
static std::vector<tokenPatterns> matching = {
    {"KeywordInt", std::regex(R"(^int\b)")},
    {"KeywordReturn", std::regex(R"(^return\b)")},
    {"Identifier", std::regex(R"(^[A-Za-z_][A-Za-z0-9_]*)")},
    // constant in the book
    {"Integer", std::regex(R"(^[0-9]+)")},
    {"LeftParen", std::regex(R"(^\()")},
    {"RightParen", std::regex(R"(^\))")},
    {"LeftBrace", std::regex(R"(^\{)")},
    {"RightBrace", std::regex(R"(^\})")},
    {"Semicolon", std::regex(R"(^;)")},
};


// "   hello    "  -> output: "hello    "
std::string start_trim(const std::string& input) {
    // index of the first non-whitespace in the str
    size_t first = input.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    } else {
        // removes everything before that index
        return input.substr(first);
    }
}


std::vector<Token> tokenise(const std::string& filewords_str) {
    // now the function gets called in main so we have the strings of words we need to tokenise

    std::vector<Token> tokens;
    size_t current_position = 0;

    while (current_position < filewords_str.length()) {
        // checks if current index is whitespace
        if (std::isspace(static_cast<unsigned char>(filewords_str[current_position]))) {
            current_position++;
        } else {
            bool found_match = false;

            std::string remaining_text = filewords_str.substr(current_position);

            for (const tokenPatterns& rule : matching) {
                std::smatch match;

                if (std::regex_search(remaining_text, match, rule.regexPattern)) {
                    std::string matched_text = match.str();
                    // create a token instance
                    Token token;
                    token.type = rule.type;
                    token.value = matched_text;

                    tokens.push_back(token);

                    current_position += matched_text.length();

                    found_match = true;
                    break;
                }
            }

            if (found_match == false) {
                std::cout << "Lexer error: unknown token starts at " << filewords_str[current_position] << std::endl;
                break;
            }
        }
    }

    return tokens;
    
}

