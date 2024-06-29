#ifndef LLVMCOMPILER_H
#define LLVMCOMPILER_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

#include "../parser/ast/ASTNodes.hpp"

// LLVM headers
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/Constant.h>

#include "./utils/utils.hpp"
#include "../errors/error.hpp"

class LLVMCompiler {
public:
    LLVMCompiler();
    void compile(Program* program);
    void printModule();
    void writeToFile(const char* filename);
private:
    uint curLambda = 0;
    std::map<std::string, uint> lambdaMap;
    std::map<std::string, llvm::Type*> typeMap;
    // Recursive function to compile ASTNodes
    llvm::Value* compileNode(ASTNode* node);
    llvm::Value* compileBinaryExpression(BinaryExpression* node);
    llvm::Value* compileIntegerLiteral(IntegerLiteral* node);
    llvm::Value* compileFloatLiteral(FloatLiteral* node);
    llvm::Value* compileStringLiteral(StringLiteral* node);
    llvm::Value* compileBooleanLiteral(BooleanLiteral* node);
    llvm::Value* compileVariableDeclaration(VariableDeclaration* node);
    llvm::Function* compileLambdaFunction(Function* node);
    llvm::Value* compileLambdaFunctionCall(VariableAccess* node);
    llvm::Value* compileVariableCaptureAccess(VariableCaptureAccess* node);
    llvm::Value* compileReturnStatement(ReturnStatement* node);

    // Compile data type to LLVM IR
    llvm::Type* compileDataType(DataType* type);
    // LLVM objects
    llvm::Module* module;
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
};

#endif