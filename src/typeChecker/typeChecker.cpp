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

DataType* TypeChecker::m_findInScope(std::string name, Scope* scope) {
    if (scope->variables.find(name) != scope->variables.end()){
        return scope->variables[name];
    }
    if (scope->parent != nullptr){
        return m_findInScope(name, scope->parent);
    }
    return nullptr;
}

int TypeChecker::checkTypes(std::vector<ASTNode*> statements, Scope* scope, DataType* returnType) {
    bool foundReturn = false;
    if (scope == nullptr){
        scope = m_globalScope;
    }
    for (ASTNode* statement : statements) {
        std::cout << "Checking: " << statement->toString() << std::endl;
        if (m_isType<Statement>(statement)) {
            std::cout << "  - Is statement" << std::endl;
            Statement* s = dynamic_cast<Statement*>(statement);
            if (m_isType<VariableDeclaration>(s)) {
                std::cout << "  - Is variable declaration" << std::endl;
                VariableDeclaration* vd = dynamic_cast<VariableDeclaration*>(s);
                if (m_findInScope(vd->name, scope) != nullptr) {
                    std::cout << "Error: Variable " << vd->name << " already declared in this scope" << std::endl;
                    return 1;
                }
                else{
                    if (m_isType<Function>(vd->value)){
                        std::cout << "    - Value is function" << std::endl;
                        Function* f = dynamic_cast<Function*>(vd->value);
                        if (f->returnType != nullptr){
                            if (f->returnType->toString() != vd->type->toString()){
                                std::cout << "Error: Function return type does not match variable type" << std::endl;
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
                    else if (m_isType<VariableAccess>(vd->value)){
                        std::cout << "    - Value is variable access" << std::endl;
                        VariableAccess* va = dynamic_cast<VariableAccess*>(vd->value);
                        if (m_findInScope(va->name, scope) == nullptr){

                            std::cout << "Error: Variable " << va->name << " not declared in this scope (access)" << std::endl;
                            return 1;
                        }
                        if (m_findInScope(va->name, scope)->toString() != vd->type->toString()){
                            std::cout << "Error: Variable " << va->name << " type does not match variable type" << std::endl;
                            return 1;
                        }                        
                    } else {
                        std::cout << "    - Value is expression" << std::endl;
                        DataType* value_dt = m_getExpression(vd->value, scope);
                        if (value_dt == nullptr){
                            return 1;
                        }
                        if (value_dt->toString() != vd->type->toString()){
                            std::cout << "Error: Variable " << vd->name << " type does not match value type" << std::endl;
                            return 1;
                        }
                    }
                }
                //std::cout << "Variable added: " << vd->name << std::endl;
                scope->variables[vd->name] = vd->type;
                scope->var_names.push_back(vd->name);
            }

        }
        else if (m_isType<VariableAssignment>(statement)){
            std::cout << "  - Is variable assignment" << std::endl;
            VariableAssignment* va = dynamic_cast<VariableAssignment*>(statement);
            DataType* va_dt = m_findInScope(va->name, scope);
            if (va_dt == nullptr) {
                std::cout << "Error: Variable " << va->name << " not declared in this scope (assignment)" << std::endl;
                return 1;
            }
            if (m_isType<Function>(va->value)){
                std::cout << "    - Value is function" << std::endl;
                Function* f = dynamic_cast<Function*>(va->value);
                if (f->returnType->toString() != va_dt->toString()){
                    std::cout << "Error: Function return type does not match variable " << va->name << " type" << std::endl;
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
                if (a != 0){
                    return a;
                }
            }
            else if(m_isType<VariableAccess>(va->value)){
                std::cout << "    - Value is variable access" << std::endl;

                VariableAccess* access = dynamic_cast<VariableAccess*>(va->value);
                DataType* access_dt = m_findInScope(access->name, scope);
                if (access_dt == nullptr){
                    std::cout << "Error: Variable " << access->name << " not declared in this scope (assignment)" << std::endl;
                    return 1;
                }
                if (access_dt->toString() != va_dt->toString()){
                    std::cout << "Error: Variable " << access->name << " type does not match variable " << va->name << " type" << std::endl;
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
                    std::cout << "Error: Variable " << va->name << " type does not match value type" << std::endl;
                    return 1;
                }
            }
            
        }
        else if (m_isType<ReturnStatement>(statement)){
            std::cout << "  - Is return statement" << std::endl;
            ReturnStatement* rs = dynamic_cast<ReturnStatement*>(statement);
            foundReturn = true;
            if (rs->value != nullptr){
                DataType* rs_dt = m_getExpression(rs->value, scope);
                if (rs_dt == nullptr){
                    return 1;
                }
                if (rs_dt->toString() != returnType->toString()){
                    std::cout << "Error: Return type does not match function return type" << std::endl;
                    return 1;
                }
            }
        }
    }
    if (!foundReturn && returnType != nullptr && returnType->toString() != "void"){
        std::cout << "Error: Non-void function of return type " << returnType->toString() << " does not return a value" << std::endl;
        return 1;
    }
    //printScope(scope);
    return 0;
}

DataType* TypeChecker::m_checkBinaryExpression(BinaryExpression* node, Scope* scope) {
    DataType* d1 = m_getExpression(node->left, scope);
    DataType* d2 = m_getExpression(node->right, scope);
    if (d1 == nullptr || d2 == nullptr){
        return nullptr;
    }
    DataType* o = m_evalOperation(d1, d2);
    if (o == nullptr){
        std::cout << "Error: Operation " << node->op << " not valid for types " << d1->toString() << " and " << d2->toString() << std::endl;
        return nullptr;
    }
    return o;
}
DataType* TypeChecker::m_getExpression(ASTNode* expr, Scope* scope) {
    if (m_isType<BinaryExpression>(expr)){
        DataType* o = m_checkBinaryExpression(dynamic_cast<BinaryExpression*>(expr), scope);
        return o;
    }
    else if (m_isType<VariableAccess>(expr)){
        VariableAccess* va = dynamic_cast<VariableAccess*>(expr);
        DataType* dt = m_findInScope(va->name, scope);
        if (dt == nullptr){
            std::cout << "Error: Variable " << va->name << " not declared in this scope (access)" << std::endl;
            return nullptr;
        }
        return dt;
    }
    else {
        DataType* dt = m_literalType(expr, scope);
        if (dt == nullptr){
            std::cout << "Error: Unknown expression type" << std::endl;
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
    if (m_isType<IntegerLiteral>(node)){
        return new IntegerType();
    }
    else if (m_isType<FloatLiteral>(node)){
        return new FloatType();
    }
    else if (m_isType<StringLiteral>(node)){
        return new StringType();
    }
    else if (m_isType<BooleanLiteral>(node)){
        return new BoolType();
    }
    else if (m_isType<Function>(node)){
        return new FunctionType();
    }
    return nullptr;
}