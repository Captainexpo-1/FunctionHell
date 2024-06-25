#include "./compiler.hpp"


void debugOut(std::string msg) {
    // Output in green color
    std::cout << "\033[1;32m" << msg << "\033[0m" << std::endl;
}

LLVMCompiler::LLVMCompiler() : builder(context) {
    module = new llvm::Module("main", context);
}

void LLVMCompiler::compile(Program* program) {
    for (ASTNode* statement : program->statements) {
        compileNode(statement);
    }
}

void LLVMCompiler::printModule() {
    module->print(llvm::outs(), nullptr);
}

llvm::Type* LLVMCompiler::compileDataType(DataType* type) {
    std::string name = type->toString();
    if (name == "int") {
        return llvm::Type::getInt32Ty(context);
    } else if (name == "float") {
        return llvm::Type::getFloatTy(context);
    } else if (name == "bool") {
        return llvm::Type::getInt1Ty(context);
    } else if (name == "string") {
        return llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0);
    } else {
        std::cerr << "Unknown data type" << std::endl;
        return nullptr;
    }
}


llvm::Value* LLVMCompiler::compileNode(ASTNode* node) {
    if (util_isType<BinaryExpression>(node)) {
        return compileBinaryExpression(dynamic_cast<BinaryExpression*>(node));
    } else if (util_isType<IntegerLiteral>(node)) {
        return compileIntegerLiteral(dynamic_cast<IntegerLiteral*>(node));
    } else if (util_isType<FloatLiteral>(node)) {
        return compileFloatLiteral(dynamic_cast<FloatLiteral*>(node));
    } else if (util_isType<StringLiteral>(node)) {
        return compileStringLiteral(dynamic_cast<StringLiteral*>(node));
    } else if (util_isType<BooleanLiteral>(node)) {
        return compileBooleanLiteral(dynamic_cast<BooleanLiteral*>(node));
    } else if (util_isType<VariableDeclaration>(node)) {
        return compileVariableDeclaration(dynamic_cast<VariableDeclaration*>(node));
    } else if (util_isType<Function>(node)) {
        return compileLambdaFunction(dynamic_cast<Function*>(node));
    } else if (util_isType<VariableAccess>(node)) {
        return compileLambdaFunctionCall(dynamic_cast<VariableAccess*>(node));
    } else if (util_isType<ReturnStatement>(node)) {
        return compileReturnStatement(dynamic_cast<ReturnStatement*>(node));
    } else if (util_isType<VariableCaptureAccess>(node)) {
        return compileVariableCaptureAccess(dynamic_cast<VariableCaptureAccess*>(node));
    } else {
        langError("Unknown ASTNode of type " + std::string(typeid(*node).name()),-1,-1);
        return nullptr;
    }
}

llvm::Value* LLVMCompiler::compileVariableCaptureAccess(VariableCaptureAccess* node) {

}

llvm::Value* LLVMCompiler::compileReturnStatement(ReturnStatement* node) {

}

llvm::Value* LLVMCompiler::compileBinaryExpression(BinaryExpression* node) {
    auto left = compileNode(node->left);
    auto right = compileNode(node->right);
    char op = node->op;
    switch (op)
    {
    case '+':
        return builder.CreateAdd(left, right, "addtmp");
    case '-':
        return builder.CreateSub(left, right, "subtmp");
    case '*':
        return builder.CreateMul(left, right, "multmp");
    case '/':
        return builder.CreateSDiv(left, right, "divtmp");
    default:
        langError("Unknown binary operator: " + std::string(1, op), -1, -1);
    }
}

llvm::Value* LLVMCompiler::compileIntegerLiteral(IntegerLiteral* node) {
    debugOut("Compiling integer literal: " + std::to_string(node->value));
    return llvm::ConstantInt::get(context, llvm::APInt(32, node->value, true));
}

llvm::Value* LLVMCompiler::compileFloatLiteral(FloatLiteral* node) {
    debugOut("Compiling float literal: " + std::to_string(node->value));
    return llvm::ConstantFP::get(context, llvm::APFloat(node->value));
}

llvm::Value* LLVMCompiler::compileStringLiteral(StringLiteral* node) {
    debugOut("Compiling string literal: " + node->value);
    return builder.CreateGlobalStringPtr(node->value);
}

llvm::Value* LLVMCompiler::compileBooleanLiteral(BooleanLiteral* node) {
    debugOut("Compiling boolean literal: " + node->toString());
    return llvm::ConstantInt::get(context, llvm::APInt(1, node->value, true));
}

llvm::Value* LLVMCompiler::compileVariableDeclaration(VariableDeclaration* node) {
    
}



llvm::Value* LLVMCompiler::compileLambdaFunction(Function* node) {
    
}

llvm::Value* LLVMCompiler::compileLambdaFunctionCall(VariableAccess* node) {

}
