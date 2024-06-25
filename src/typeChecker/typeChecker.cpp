#include "typeChecker.hpp"


TypeChecker::TypeChecker() {
    m_globalScope = new Scope();
}

void TypeChecker::printScope(Scope* scope) {
    std::string o = "Scope {";
    for (std::string v : scope->var_names){
        o += "\n   " + v + " -> " + scope->variables[v]->toString();   
    }
    o += "\n}";
    std::cout << o << std::endl;
}



DataType* TypeChecker::m_findInImmediateScope(std::string name, Scope* scope) {
    if (scope->variables.find(name) != scope->variables.end()){
        return scope->variables[name];
    }
    return nullptr;
}

DataType* TypeChecker::m_findInHigherScope(std::string name, Scope* scope) {
    if (scope->parent != nullptr){
        return m_findInScope(name, scope->parent);
    }
    return nullptr;
}

DataType* TypeChecker::m_findInScope(std::string name, Scope* scope) {
    DataType* dt = m_findInImmediateScope(name, scope);
    if (dt != nullptr){
        return dt;
    }
    return m_findInHigherScope(name, scope);
}

int TypeChecker::checkTypes(std::vector<ASTNode*> statements, Scope* scope, DataType* returnType) {
    bool foundReturn = false;
    if (scope == nullptr){
        scope = m_globalScope;
    }
    for (ASTNode* statement : statements) {
        std::cout << "Checking: " << statement->toString() << std::endl;
        if (util_isType<Statement>(statement)) {
            std::cout << "  - Is statement" << std::endl;
            Statement* s = dynamic_cast<Statement*>(statement);
            if (util_isType<VariableDeclaration>(s)) {
                std::cout << "  - Is variable declaration" << std::endl;
                VariableDeclaration* vd = dynamic_cast<VariableDeclaration*>(s);
                if (m_findInScope(vd->name, scope) != nullptr) {
                    langError("Error: Variable " + vd->name + " already declared in this scope",-1,-1);
                    return 1;
                }
                else{
                    if (util_isType<Function>(vd->value)){
                        std::cout << "    - Value is function" << std::endl;
                        Function* f = dynamic_cast<Function*>(vd->value);
                        if (f->returnType != nullptr){
                            if (f->returnType->toString() != vd->type->toString()){
                                langError("Error: Function return type does not match variable type",-1,-1);
                                return 1;
                            }
                        }
                        Scope* funcScope = new Scope();
                        funcScope->parent = scope;
                        // Add func params to scope
                        for (FunctionParameter* fp : f->params){
                            funcScope->variables[fp->name] = fp->type;
                            funcScope->var_names.push_back(fp->name);
                            std::cout << "Variable added: " << fp->name << " " << fp->type->toString() << std::endl;
                        }
                        // Recursively type check the function
                        printScope(funcScope);
                        int a = checkTypes(f->body, funcScope, f->returnType);
                        if (a != 0){
                            return a;
                        }
                    }
                    else if (util_isType<VariableAccess>(vd->value)){
                        std::cout << "    - Value is variable access" << std::endl;
                        VariableAccess* va = dynamic_cast<VariableAccess*>(vd->value);
                        if (m_findInScope(va->name, scope) == nullptr){

                            langError("Error: Variable " + va->name + " not declared in this scope (access)",-1,-1);
                            return 1;
                        }
                        if (m_findInScope(va->name, scope)->toString() != vd->type->toString()){
                            langError("Error: Variable " + va->name + " type does not match variable type",-1,-1);
                            return 1;
                        }                        
                    } else {
                        std::cout << "    - Value is expression" << std::endl;
                        DataType* value_dt = m_getExpression(vd->value, scope);
                        if (value_dt == nullptr){
                            return 1;
                        }
                        if (value_dt->toString() != vd->type->toString()){
                            langError("Error: Variable " + vd->name + " type does not match value type",-1,-1);
                            return 1;
                        }
                    }
                }
                //std::cout << "Variable added: " << vd->name << std::endl;
                scope->variables[vd->name] = vd->type;
                scope->var_names.push_back(vd->name);
            }
            else if (util_isType<ReturnStatement>(s)){
                std::cout << "  - Is return statement" << std::endl;
                ReturnStatement* rs = dynamic_cast<ReturnStatement*>(s);
                foundReturn = true;
                if (rs->value != nullptr){
                    DataType* rs_dt = m_getExpression(rs->value, scope);
                    if (rs_dt == nullptr){
                        return 1;
                    }
                    if (rs_dt->toString() != returnType->toString()){
                        langError("Error: Return type does not match function return type",-1,-1);
                        return 1;
                    }
                }
            }
            else {
                std::cout << "  - Is expression" << std::endl;
                DataType* dt = m_getExpression(s, scope);
                if (dt == nullptr){
                    return 1;
                }
            }

        }
        else if (util_isType<VariableAssignment>(statement)){
            std::cout << "  - Is variable assignment" << std::endl;
            VariableAssignment* va = dynamic_cast<VariableAssignment*>(statement);
            DataType* va_dt = m_findInScope(va->name, scope);
            if (va_dt == nullptr) {
                langError("Error: Variable " + va->name + " not declared in this scope (assignment)",-1,-1);
                return 1;
            }
            if (util_isType<Function>(va->value)){
                std::cout << "    - Value is function" << std::endl;
                Function* f = dynamic_cast<Function*>(va->value);
                if (f->returnType->toString() != va_dt->toString()){
                    langError("Error: Function return type does not match variable " + va->name + " type",-1,-1);
                    return 1;
                }

                Scope* funcScope = new Scope();
                funcScope->parent = scope;
                // Add func params to scope
                for (FunctionParameter* fp : f->params){
                    funcScope->variables[fp->name] = fp->type;
                    funcScope->var_names.push_back(fp->name);
                }
                // Recursively type check the function
                printScope(funcScope);
                int a = checkTypes(f->body, funcScope, f->returnType);
                std::vector<std::tuple<std::string, DataType*>> captures = getCaptures(f->body, funcScope);
                if (a != 0){
                    return a;
                }
            }
            else if(util_isType<VariableAccess>(va->value)){
                std::cout << "    - Value is variable access" << std::endl;

                VariableAccess* access = dynamic_cast<VariableAccess*>(va->value);
                DataType* access_dt = m_findInScope(access->name, scope);
                if (access_dt == nullptr){
                    langError("Error: Variable " + access->name + " not declared in this scope (assignment)",-1,-1);
                    return 1;
                }
                if (access_dt->toString() != va_dt->toString()){
                    langError("Error: Variable " + access->name + " type does not match variable " + va->name + " type",-1,-1);
                    return 1;
                }
            }
            else{
                std::cout << "    - Value is expression" << std::endl;
                DataType* value_dt = m_getExpression(va->value, scope);
                if (value_dt == nullptr){
                    return 1;
                }
                if (value_dt->toString() != va_dt->toString()){
                    langError("Error: Variable " + va->name + " type does not match value type",-1,-1);
                    return 1;
                }
            }
            
        }
        else if (util_isType<ReturnStatement>(statement)){
            std::cout << "  - Is return statement" << std::endl;
            ReturnStatement* rs = dynamic_cast<ReturnStatement*>(statement);
            foundReturn = true;
            if (rs->value != nullptr){
                DataType* rs_dt = m_getExpression(rs->value, scope);
                if (rs_dt == nullptr){
                    return 1;
                }
                if (rs_dt->toString() != returnType->toString()){
                    langError("Error: Return type does not match function return type",-1,-1);
                    return 1;
                }
            }
        }
        else {
            std::cout << "  - Is expression" << std::endl;
            DataType* dt = m_getExpression(statement, scope);
            if (dt == nullptr){
                return 1;
            }
        
        }
    }
    if (!foundReturn && returnType != nullptr && returnType->toString() != "void"){
        langError("Error: Non-void function of return type " + returnType->toString() + " does not return a value",-1,-1);
        return 1;
    }
    //printScope(scope);
    return 0;
}

std::vector<std::tuple<std::string, DataType*>> TypeChecker::getCaptures(std::vector<ASTNode*> statements, Scope* scope) {
    std::vector<std::tuple<std::string, DataType*>> captures;
    for (ASTNode* statement : statements) {
        if (util_isType<VariableAccess>(statement)){
            VariableAccess* va = dynamic_cast<VariableAccess*>(statement);
            if (m_findInScope(va->name, scope) == nullptr){
                captures.push_back(std::make_tuple(va->name, nullptr));
            }
        }
        else if (util_isType<VariableAssignment>(statement)){
            VariableAssignment* va = dynamic_cast<VariableAssignment*>(statement);
            if (m_findInScope(va->name, scope) == nullptr){
                captures.push_back(std::make_tuple(va->name, nullptr));
            }
        }
        else if (util_isType<Function>(statement)){
            Function* f = dynamic_cast<Function*>(statement);
            Scope* funcScope = new Scope();
            funcScope->parent = scope;
            // Add func params to scope
            for (FunctionParameter* fp : f->params){
                funcScope->variables[fp->name] = fp->type;
                funcScope->var_names.push_back(fp->name);
            }
            // Recursively type check the function
            printScope(funcScope);
            std::vector<std::tuple<std::string, DataType*>> a = getCaptures(f->body, funcScope);
            for (std::tuple<std::string, DataType*> t : a){
                captures.push_back(t);
            }
        }
    }
    return captures;
}

DataType* TypeChecker::m_checkBinaryExpression(BinaryExpression* node, Scope* scope) {
    DataType* d1 = m_getExpression(node->left, scope);
    DataType* d2 = m_getExpression(node->right, scope);
    if (d1 == nullptr || d2 == nullptr){
        return nullptr;
    }
    DataType* o = m_evalOperation(d1, d2);
    if (o == nullptr){
        langError("Error: Operation " + node->op + " not valid for types " + d1->toString() + " and " + d2->toString(),-1,-1);
        return nullptr;
    }
    return o;
}
DataType* TypeChecker::m_getExpression(ASTNode* expr, Scope* scope) {
    if (util_isType<BinaryExpression>(expr)){
        DataType* o = m_checkBinaryExpression(dynamic_cast<BinaryExpression*>(expr), scope);
        return o;
    }
    else if (util_isType<VariableAccess>(expr)){
        VariableAccess* va = dynamic_cast<VariableAccess*>(expr);
        DataType* dt = m_findInImmediateScope(va->name, scope);
        if (dt == nullptr){
            langError("Error: Variable " + va->name + " not declared in this scope (access)",-1,-1);
            return nullptr;
        }
        return dt;
    }
    else if (util_isType<VariableCaptureAccess>(expr)){
        VariableCaptureAccess* vca = dynamic_cast<VariableCaptureAccess*>(expr);
        DataType* dt = m_findInHigherScope(vca->access->name, scope);
        vca->type = dt;
        if (dt == nullptr){
            langError("Error: Variable capture " + vca->access->name + " not declared in higher (capture access)",-1,-1);
            return nullptr;
        }
        return dt;
    }
    else {
        DataType* dt = m_literalType(expr, scope);
        if (dt == nullptr){
            langError("Error: Unknown expression type",-1,-1);
            return nullptr;
        }  
        return dt;
    }
    return nullptr;
}

DataType* TypeChecker::m_evalOperation(DataType* d1, DataType* d2){
    std::string s_d1 = d1->toString();
    std::string s_d2 = d2->toString();

    std::map <std::string, DataType*> ops = {
        {"int int", new IntegerType()},
        {"int float", new FloatType()},
        {"float float", new FloatType()},
        {"string string", new StringType()},
        //{"bool bool", new BoolType()}, // true + true = true, false + false = false, true + false = false, false + true = false
        //{"int bool", new BoolType()},
        //{"bool int", new BoolType()},
        //{"float bool", new BoolType()},
        //{"bool float", new BoolType()},
    };
    if (ops.find(s_d1 + " " + s_d2) != ops.end()){
        return ops[s_d1 + " " + s_d2];
    }
    else if(ops.find(s_d2 + " " + s_d1) != ops.end()){
        return ops[s_d2 + " " + s_d1];
    }
    return nullptr;
}

DataType* TypeChecker::m_literalType(ASTNode* node, Scope* scope) {
    if (util_isType<IntegerLiteral>(node)){
        return new IntegerType();
    }
    else if (util_isType<FloatLiteral>(node)){
        return new FloatType();
    }
    else if (util_isType<StringLiteral>(node)){
        return new StringType();
    }
    else if (util_isType<BooleanLiteral>(node)){
        return new BoolType();
    }
    else if (util_isType<Function>(node)){
        return new FunctionType();
    }
    return nullptr;
}