#include "./compiler.hpp"
	

void debugOut(std::string msg) {
    // Output in green color
    std::cout << "\033[1;32m" << msg << "\033[0m" << std::endl;
}
	
void LLVMCompiler::writeToFile(const char* filename) {
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
	
    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message();
        return;
    }
	
    module->print(dest, nullptr);
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
    uint id = 0;
} closureInfo;
typedef struct {
    std::vector<varInfo*> params;
    std::vector<varInfo*> captures;
    llvm::Type* returnType;
    closureInfo* closure;
    uint id = 0;
} lambdaInfo;


std::string varInfoStr(varInfo* vi){
    return "VarInfo(name=\"" + vi->name + "\")";
}

llvm::Function* LLVMCompiler::compileLambdaFunction(Function* func) {
    /*debugOut("Compiling lambda function with captures");
	
    // Collect parameter types including captures
    std::vector<llvm::Type*> paramTypes;
    for (FunctionParameter* param : func->params) {
        paramTypes.push_back(compileDataType(param->type));
    }
    for (VariableCaptureAccess* capture : func->captures) {
        paramTypes.push_back(compileDataType(capture->type));
    }
	
    auto oldIP = builder.GetInsertBlock();
	
    // Create the function type
    llvm::FunctionType* functionType = llvm::FunctionType::get(compileDataType(func->returnType), paramTypes, false);
	
    // Create the function
    llvm::Function* llvmFunction = llvm::Function::Create(
        functionType, llvm::Function::ExternalLinkage, "lambda_" + std::to_string(func->captures.size()), module);
	
    // Create the entry block and set the builder's insert point to it
    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(context, "entry", llvmFunction);
    builder.SetInsertPoint(entryBlock);
	
    // Allocate space for parameters and captures, then store the values
    unsigned idx = 0;
    for (auto& arg : llvmFunction->args()) {
        std::string argName;
        if (idx < func->params.size()) {
            argName = func->params[idx]->name;
        } else {
            argName = func->captures[idx - func->params.size()]->access->name;
        }
        arg.setName(argName);
        llvm::AllocaInst* alloca = builder.CreateAlloca(arg.getType(), nullptr, arg.getName());
        builder.CreateStore(&arg, alloca);
        ++idx;
    }
	
    // Compile the body of the lambda function
    for (ASTNode* statement : func->body) {
        compileNode(statement);
    }
	
    builder.SetInsertPoint(oldIP);
	
    return llvmFunction;*/
    debugOut("Compiling lambda function:");
    curLambda++;

    debugOut("\t- Creating closure struct");
    closureInfo* closure = new closureInfo();
    closure->id = curLambda;

    debugOut("\t- Creating funcinfo struct");
    lambdaInfo* funcInfo = new lambdaInfo();
    funcInfo->returnType = compileDataType(func->returnType);
    funcInfo->id = curLambda;
    funcInfo->closure = closure;
    
    debugOut("\t- Getting params");
    //std::vector<varInfo*> paramInfo;
    std::vector<llvm::Type*> paramTypes;
    int i = 0;
    for(FunctionParameter* param : func->params){
        //varInfo* vi = new varInfo();
        //vi->name = param->name;
        //vi->type = compileDataType(param->type);
        //debugOut("\t\t- Got v_inf: " + varInfoStr(vi));
        //paramInfo.push_back(vi);
        paramTypes.push_back(compileDataType(param->type));
        i++;
    }
    debugOut("\t- Creating function");
    llvm::FunctionType* functionType = llvm::FunctionType::get(compileDataType(func->returnType), paramTypes, false);
    llvm::Function* llvmFunction = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, "lambda_" + std::to_string(funcInfo->id), module);
	
    debugOut("\t- Compiling body");
    auto oldIP = builder.GetInsertBlock();
    // Create the entry block and set the builder's insert point to it
    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(context, "entry", llvmFunction);
    builder.SetInsertPoint(entryBlock);

	// Allocate space for parameters and captures, then store the values
    unsigned idx = 0;
    for (auto& arg : llvmFunction->args()) {
        std::string argName;
        if (idx < func->params.size()) {
            argName = func->params[idx]->name;
        } else {
            argName = func->captures[idx - func->params.size()]->access->name;
        }
        arg.setName(argName);
        llvm::AllocaInst* alloca = builder.CreateAlloca(arg.getType(), nullptr, arg.getName());
        builder.CreateStore(&arg, alloca);
        ++idx;
    }
	
    // Compile the body of the lambda function
    for (ASTNode* statement : func->body) {
        compileNode(statement);
    }
	
    builder.SetInsertPoint(oldIP);
	
    return llvmFunction;
}
	

llvm::Value* LLVMCompiler::compileLambdaFunctionCall(VariableAccess* node) {
}
