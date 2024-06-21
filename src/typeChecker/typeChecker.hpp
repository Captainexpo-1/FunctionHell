#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "../parser/ast/ASTNodes.hpp"

struct Scope {
    std::vector<std::string> var_names;
    std::map<std::string, DataType*> variables;
    Scope* parent;
};



class TypeChecker {
public:
    TypeChecker();
    int checkTypes(std::vector<ASTNode*> statements, Scope* scope, DataType* returnType); // Returns 0 if no errors, 1 if errors
    void printScope(Scope* scope);
private:
    template <typename T> 
    bool m_isType(ASTNode* node);
    DataType* m_findInScope(std::string name, Scope* scope);
    Scope* m_globalScope;
};

#include "typeChecker.inl"

#endif