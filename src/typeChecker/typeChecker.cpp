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
        if (m_isType<Statement>(statement)) {
            Statement* s = dynamic_cast<Statement*>(statement);
            if (m_isType<VariableDeclaration>(s)) {
                VariableDeclaration* vd = dynamic_cast<VariableDeclaration*>(s);
                if (m_findInScope(vd->name, scope) != nullptr) {
                    std::cout << "Error: Variable " << vd->name << " already declared in this scope" << std::endl;
                    return 1;
                }
                else{
                    if (m_isType<Function>(vd->value)){
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
                        VariableAccess* va = dynamic_cast<VariableAccess*>(vd->value);
                        if (m_findInScope(va->name, scope) == nullptr){

                            std::cout << "Error: Variable " << va->name << " not declared in this scope (access)" << std::endl;
                            return 1;
                        }
                        if (m_findInScope(va->name, scope)->toString() != vd->type->toString()){
                            std::cout << "Error: Variable " << va->name << " type does not match variable type" << std::endl;
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
            
            VariableAssignment* va = dynamic_cast<VariableAssignment*>(statement);
            std::cout << "Variable Assignment: " << va->name << std::endl;
            DataType* va_dt = m_findInScope(va->name, scope);
            if (va_dt == nullptr) {
                std::cout << "Error: Variable " << va->name << " not declared in this scope (assignment)" << std::endl;
                return 1;
            }
            if (m_isType<Function>(va->value)){
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
        }
        else if (m_isType<ReturnStatement>(statement)){
            ReturnStatement* rs = dynamic_cast<ReturnStatement*>(statement);
            // TODO: Implement return statement checking
            // Temp fix to keep compiler from throwing warnings
            rs->toString();
            foundReturn = true;
        }
    }
    if (!foundReturn && returnType != nullptr && returnType->toString() != "void"){
        std::cout << "Error: Non-void function of return type " << returnType->toString() << " does not return a value" << std::endl;
        return 1;
    }
    //printScope(scope);
    return 0;
}
