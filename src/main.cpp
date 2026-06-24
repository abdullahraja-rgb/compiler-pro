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
#include "lexer.cpp"


int main(int argc, char* argv[]) {

    std::cout << "Basic Command Line Program" << std::endl;
    std::cout << "Enter the C File Name: " << std::endl;
    bool done = false;
    std::string filename;


    while (done != true) {
        if (argc != 1) {
            std::cout << "Invalid input only enter C Filename" << std::endl;
        } else {
            std::cin >> filename;
            // std::cout << "Filename:" << filename << std::endl;

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

    


                // now we have stored the string in filewords we now need to pass this to our lexer
                        
                    } else {
                        std::cout << "Its not a c file. Enter a C Filename: " << std::endl;
                    }
            


        }
    }

    

}

