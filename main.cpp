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


void printHelp(char** argv){
    std::cout << "Usage: " << argv[0] << " <filename>" << " -o" << " <output_file>" << std::endl;
    
}

void runCMD(const char* cmd){
    std::cout << cmd << std::endl;
    system(cmd);
}

int main(int argc, char** argv) {
    char* output_file = "output.ts";
    bool runOutput = false;
    bool rmJSOut = true;
    if (argc < 2 || argc == 0){
        printHelp(argv);
        exit(1);
    }
    for(int i = 0; i < argc; i++){
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
            printHelp(argv);
            exit(0);
        }
        else if(strcmp(argv[i], "-o") == 0){
            output_file = argv[i+1];
            
        }
        else if(strcmp(argv[i], "-r") == 0){
            runOutput = true;
        }
        else if(strcmp(argv[i], "-noRm") == 0){
            rmJSOut = false;
        }
    }
    Lexer lexer;
    Parser parser;
    std::string source = readFile(argv[1]);
    setSource(source);
    std::vector<Token> tokens = lexer.tokenize(source);
    Program* program = parser.parse(tokens);
    //program->print();
    //std::cout << "------------------ Transpiling ------------------\n" << std::endl;
    Transpiler transpiler;
    std::string output = transpiler.transpile(program->statements);

    std::ofstream file(output_file);
    file << output;
    file.close();
    //std::cout << "Transpiled code written to " << output_file << std::endl;
    if (runOutput){
    
        runCMD(("tsc " + std::string(output_file)).c_str());
        std::string o_file = std::string(output_file).substr(0, std::string(output_file).size()-3) + ".js"; 
        runCMD(("node " + o_file).c_str());
        if(rmJSOut) runCMD(("rm " + o_file).c_str());
    } 
    return 0;
}


