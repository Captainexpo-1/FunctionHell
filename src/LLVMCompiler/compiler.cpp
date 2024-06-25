#include "./compiler.hpp"


void debugOut(std::string msg) {
    // Output in green color
    std::cout << "\033[1;32m" << msg << "\033[0m" << std::endl;
}

LLVMCompiler::LLVMCompiler() : builder(context) {
    module = new llvm::Module("main", context);
    // Create the main function
    llvm::FunctionType* mainFunctionType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::Function* mainFunction = llvm::Function::Create(mainFunctionType, llvm::Function::ExternalLinkage, "main", module);
    llvm::BasicBlock* block = llvm::BasicBlock::Create(context, "entry", mainFunction);
    builder.SetInsertPoint(block);
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
    return compileNode(node->access);
}

llvm::Value* LLVMCompiler::compileReturnStatement(ReturnStatement* node) {
    return builder.CreateRet(compileNode(node->value));
}

llvm::Value* LLVMCompiler::compileBinaryExpression(BinaryExpression* node) {
    auto left = compileNode(node->left);
    auto right = compileNode(node->right);
    char op = node->op[0];
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
    debugOut("Compiling variable declaration: " + node->name);
    llvm::Type* type = compileDataType(node->type);
    debugOut("Type: " + node->type->toString());
    llvm::Value* value = compileNode(node->value);
    debugOut("Value: " + node->value->toString());
    llvm::AllocaInst* alloca = builder.CreateAlloca(type, nullptr, node->name);
    debugOut("Alloca: " + node->name);
    if (builder.GetInsertBlock() == nullptr) {
        langError("No block to insert into", -1, -1);
    }
    builder.CreateStore(value, alloca);
    debugOut("Store: " + node->name);
    return alloca;
}



typedef struct {
    llvm::Type* type;
    std::string name;
} varInfo;
typedef struct {
    std::vector<varInfo*> captures;
    std::string id;
    uint intID = 0;
} closureInfo;
typedef struct {
    std::vector<varInfo*> params;
    std::vector<varInfo*> captures;
    llvm::Type* returnType;
    std::vector<ASTNode*> body;
    closureInfo* closure;
    std::string id;
    uint intID = 0;
} lambdaInfo;


llvm::Value* LLVMCompiler::compileLambdaFunction(Function* node) {
    std::vector<varInfo*> paramInfos;
    std::vector<varInfo*> captureInfos;

    for (FunctionParameter* param : node->params) {
        varInfo* info = new varInfo();
        info->type = compileDataType(param->type);
        info->name = param->name;
        paramInfos.push_back(info);
    }

    for (ASTNode* statement : node->body) {
        if (util_isType<VariableCaptureAccess>(statement)) {
            VariableCaptureAccess* access = dynamic_cast<VariableCaptureAccess*>(statement);
            varInfo* info = new varInfo();
            info->type = compileDataType(access->type);
            info->name = access->access->name;
            captureInfos.push_back(info);
        }
    }

    curLambda++;

    closureInfo* closure = new closureInfo();
    closure->captures = captureInfos;
    closure->id = "closure_" + std::to_string(curLambda);
    closure->intID = curLambda;

    std::vector<llvm::Type*> closureTypes;
    for (varInfo* capture : captureInfos) {
        closureTypes.push_back(capture->type);
    }
    llvm::StructType* closureType = llvm::StructType::create(context, closureTypes, closure->id);

    // Make sure to add the closure type to the module
    module->getOrInsertGlobal(closure->id, closureType);

    llvm::Type* returnType = compileDataType(node->returnType);

    lambdaInfo* info = new lambdaInfo();
    info->params = paramInfos;
    info->captures = captureInfos;
    info->returnType = returnType;
    info->body = node->body;
    info->id = "lambda_" + std::to_string(curLambda);
    info->intID = curLambda;
    info->closure = closure;

    std::vector<llvm::Type*> paramTypes;
    for (varInfo* param : paramInfos) {
        paramTypes.push_back(param->type);
    }

    llvm::FunctionType* functionType = llvm::FunctionType::get(returnType, paramTypes, false);
    llvm::Function* function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, info->id, module);
    llvm::BasicBlock* block = llvm::BasicBlock::Create(context, "entry", function);

    auto oldBlock = builder.GetInsertBlock();
    builder.SetInsertPoint(block);

    std::vector<llvm::Value*> captures;
    for (int i = 0; i < captureInfos.size(); i++) {
        llvm::AllocaInst* alloca = builder.CreateAlloca(captureInfos[i]->type, nullptr, std::to_string(i));
        captures.push_back(alloca);
    }

    int index = 0;
    for (llvm::Value* capture : captures) {
        llvm::Value* ptr = builder.CreateStructGEP(closureType, function->arg_begin(), index++);
        llvm::Value* value = builder.CreateLoad(captureInfos[index]->type, ptr);
        builder.CreateStore(value, capture);
    }

    std::vector<llvm::Value*> params;
    for (varInfo* param : paramInfos) {
        llvm::AllocaInst* alloca = builder.CreateAlloca(param->type, nullptr, param->name);
        params.push_back(alloca);
    }

    index = 0;
    for (llvm::Argument& arg : function->args()) {
        builder.CreateStore(&arg, params[index++]);
    }

    for (ASTNode* statement : info->body) {
        compileNode(statement);
    }

    builder.SetInsertPoint(oldBlock);

    return function;
}

llvm::Value* LLVMCompiler::compileLambdaFunctionCall(VariableAccess* node) {

}
