// program summary: reads a c files content
#include <iostream>
#include <iomanip>
#include <string>
#include <filesystem>
// file handling 
#include <fstream>


int main(int argc, char* argv[]) {

    std::cout << "Basic Command Line Program" << std::endl;
    std::cout << "Enter the C File Name: " << std::endl;
    bool done = false;
    std::string filename;

    std::filesystem::path filePath(filename);

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
                
                std::string filelines;
                while (std::getline(file, filelines)) {
                    std::cout << filelines << std::endl;

                }
                        
                    } else {
                        std::cout << "Its not a c file. Enter a C Filename: " << std::endl;
                    }
            


        }
    }

    

}

