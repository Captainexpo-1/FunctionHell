#include <iostream>
#include <fstream>
#include "./src/lexer/lexer.hpp"
#include "./src/parser/parser.hpp"
#include "./src/errors/error.hpp"
#include "./src/transpiler/transpiler.hpp"

std::string readFile(const char* filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        exit(1);
    }
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return source;
}


int main(int argc, char** argv) {
    char* output_file = NULL;
    if (argc < 2 || argc == 0){
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        exit(1);
    }
    for(int i = 0; i < argc; i++){
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
            std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
            exit(0);
        }
        else if(strcmp(argv[i], "-o") == 0){
            output_file = argv[i+1];
            
        }
    }
    Lexer lexer;
    Parser parser;
    std::string source = readFile(argv[1]);
    setSource(source);
    std::vector<Token> tokens = lexer.tokenize(source);
    Program* program = parser.parse(tokens);
    program->print();
    std::cout << "------------------ Transpiling ------------------\n" << std::endl;
    Transpiler transpiler;
    std::string output = transpiler.transpile(program->statements);
    if(output_file != NULL){
        std::ofstream file(output_file);
        file << output;
        file.close();
    }
    else{
        // Output to default file
        std::ofstream file("output.js");
        file << output;
        file.close();
    }

    return 0;
}