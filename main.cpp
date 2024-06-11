#include <iostream>
#include <fstream>
#include "./src/lexer/lexer.hpp"
#include "./src/parser/parser.hpp"
#include "./src/errors/error.hpp"

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
    if (argc < 2 || argc == 0){
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        exit(1);
    }
    Lexer lexer;
    Parser parser;
    std::string source = readFile(argv[1]);
    setSource(source);
    std::vector<Token> tokens = lexer.tokenize(source);
    Program* program = parser.parse(tokens);
    program->print();
    return 0;
}