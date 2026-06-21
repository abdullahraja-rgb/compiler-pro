// Placeholder for the compiler command-line entry point.
// Future CLI: read a source file, compile it, and write assembly output.
#include <iostream>
#include <iomanip>
#include <string>

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
            std::cout << filename << std::endl;
            


        }
    }

    

}

