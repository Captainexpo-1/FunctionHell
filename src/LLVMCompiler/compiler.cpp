#include "./compiler.hpp"


void debugOut(std::string msg) {
    // Output in green color
    std::cout << "\033[1;32m" << msg << "\033[0m" << std::endl;
}

LLVMCompiler::LLVMCompiler() : builder(context) {
    module = new llvm::Module("main", context);
    // Create the main function and set it as the current function
    llvm::FunctionType* mainFuncType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
    llvm::Function* mainFunc = llvm::Function::Create(mainFuncType, llvm::Function::ExternalLinkage, "main", module);
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(entry);
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
    debugOut("Compiling return statement: " + node->toString());
    llvm::Value* val = compileNode(node->value);
    if (val == nullptr) {
        return nullptr;
    }
    debugOut("Creating return instruction");
    builder.CreateRet(val);
    return val;
}

llvm::Value* LLVMCompiler::compileBinaryExpression(BinaryExpression* node) {
    llvm::Value* left = compileNode(node->left);
    llvm::Value* right = compileNode(node->right);

    if (left == nullptr || right == nullptr) {
        return nullptr;
    }
    debugOut("Compiling binary expression: " + node->toString());
    const char* op = node->op.c_str();
    if (strcmp(op, "+") == 0) {
        return builder.CreateAdd(left, right, std::string("addtmp"));
    } else if (strcmp(op, "-") == 0) {
        return builder.CreateSub(left, right, std::string("subtmp"));
    } else if (strcmp(op, "*") == 0) {
        return builder.CreateMul(left, right, std::string("multmp"));
    } else if (strcmp(op, "/") == 0) {
        return builder.CreateSDiv(left, right, std::string("divtmp"));
    } else if (strcmp(op, "%") == 0) {
        return builder.CreateSRem(left, right, std::string("modtmp"));
    } else {
        std::cerr << "Unknown binary operator" << std::endl;
        return nullptr;
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
    if (!node) {
        langError("node is null",-1,-1);
        return nullptr;
    }

    debugOut("Compiling variable declaration: " + node->name);
    
    llvm::Value* val = nullptr;
    if (node->value != nullptr) {
        debugOut("Compiling variable value: " + node->value->toString());
        val = compileNode(node->value);
    }
    
    if (!node->type) {
        langError("node->type is null",-1,-1);
        return nullptr;
    }

    debugOut("Compiling variable type: " + node->type->toString());
    llvm::Type *type = compileDataType(node->type);
    
    if (!type) {
        langError("compileDataType returned null for type: " + node->type->toString(),-1,-1);
        return nullptr;
    }

    debugOut("Creating alloca for variable: " + node->name);
    debugOut("Variable type: " + std::to_string(type->getTypeID()));

    // Ensure the builder has a valid insertion point
    if (!builder.GetInsertBlock()) {
        langError("builder has no valid insertion point",-1,-1);
        return nullptr;
    }

    llvm::AllocaInst *alloca = builder.CreateAlloca(type, nullptr, node->name.c_str());
    debugOut("Alloca created for variable: " + node->name);
    
    if (!alloca) {
        langError("alloca creation failed for variable: " + node->name,-1,-1);
        return nullptr;
    }

    debugOut("Storing variable in scope: " + node->name);
    if (val != nullptr) {
        debugOut("Storing value in variable: " + node->name);
        builder.CreateStore(val, alloca);
    }

    return alloca;
}



llvm::Value* LLVMCompiler::compileLambdaFunction(Function* node) {
    /*
    The structure of lambda functions as implemented in this compiler is as follows:
    1. Create a global closure struct for the captured variables (captures are node->captures, capture types are node->captures[i]->type)
    2. Create the closure and store the captured variables
    3. Create a global function with the same id as the closure
    4. Compile the function body
    5. Create a global function pointer
    6. Return the function pointer
    */
    debugOut("Compiling lambda function: " + node->toString());

    // Step 1: Create a struct type for the captured variables
    debugOut("Creating closure struct type");
    std::vector<llvm::Type*> captureTypes;
    for (auto capture : node->captures) {
        captureTypes.push_back(compileDataType(capture->type));
    }
    llvm::StructType* closureType = llvm::StructType::create(context, captureTypes, "closure");

    debugOut("Closure struct type created");
    // Step 2: Create the closure and store the captured variables
    llvm::Value* closure = builder.CreateAlloca(closureType, nullptr, "closure");
    debugOut("Closure created");
    for (size_t i = 0; i < node->captures.size(); ++i) {
        llvm::Value* captureValue = compileNode(node->captures[i]);
        if (captureValue == nullptr) {
            langError("captureValue is null",-1,-1);
            return nullptr;
        }
        debugOut("Capturing variable: " + (node->captures[i]->access ? node->captures[i]->access->name : ""));
        llvm::Value* capturePtr = builder.CreateConstInBoundsGEP2_32(closureType, closure, 0, i);
        debugOut("Captured variable pointer created");
        if (builder.GetInsertBlock() == nullptr) {
            langError("builder has no valid insertion point",-1,-1);
            return nullptr;
        }
        builder.CreateStore(captureValue, capturePtr);
        debugOut("Captured variable stored in closure");
    }       

    debugOut("Closure created and captured variables stored");
    // Step 3: Create the lambda function
    std::vector<llvm::Type*> paramTypes;
    for (auto param : node->params) {
        paramTypes.push_back(compileDataType(param->type));
    }
    llvm::FunctionType* funcType = llvm::FunctionType::get(compileDataType(node->returnType), paramTypes, false);
    llvm::Function* lambdaFunction = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, node->name, module);

    debugOut("Lambda function created");
    // Set up a new basic block and builder for the function body
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", lambdaFunction);
    llvm::IRBuilder<> funcBuilder(entry);

    debugOut("Basic block and builder created");
    // Step 4: Compile the function body
    // Set up function arguments
    size_t idx = 0;
    for (auto& arg : lambdaFunction->args()) {
        arg.setName(node->params[idx++]->name);
    }

    // Compile the function body
    auto prevInsertPoint = builder.saveIP();

    builder.SetInsertPoint(entry);
    for (ASTNode* statement : node->body) {
        compileNode(statement);
    }
    
    builder.restoreIP(prevInsertPoint);
    // Step 5: Create a global function pointer
    llvm::PointerType* funcPtrType = llvm::PointerType::get(funcType, 0);
    llvm::GlobalVariable* funcPtr = new llvm::GlobalVariable(*module, funcPtrType, false, llvm::GlobalValue::ExternalLinkage, lambdaFunction, node->name + "_ptr");


    // Step 6: Return the function pointer
    return funcPtr;
}

llvm::Value* LLVMCompiler::compileLambdaFunctionCall(VariableAccess* node) {
    debugOut("Compiling lambda function call: " + node->name);
    llvm::Function* func = module->getFunction(node->name);
    if (func == nullptr) {
        langError("Function not found: " + node->name,-1,-1);
        return nullptr;
    }

    std::vector<llvm::Value*> args;
    for (auto arg : node->args) {
        llvm::Value* val = compileNode(arg);
        if (val == nullptr) {
            langError("Argument value is null",-1,-1);
            return nullptr;
        }
        args.push_back(val);
    }

    return builder.CreateCall(func, args, "calltmp");
}
