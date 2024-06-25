#ifndef TYPECHECKER_H
#define TYPECHECKER_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "../parser/ast/ASTNodes.hpp"

#include "../errors/error.hpp"
#include "../utils/utils.hpp"


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
    DataType* m_findInScope(std::string name, Scope* scope);
    DataType* m_findInImmediateScope(std::string name, Scope* scope);
    DataType* m_findInHigherScope(std::string name, Scope* scope);
    Scope* m_globalScope;
    DataType* m_getExpression(ASTNode* node, Scope* scope);
    DataType* m_checkBinaryExpression(BinaryExpression* node, Scope* scope);
    DataType* m_literalType(ASTNode* node, Scope* scope);
    DataType* m_evalOperation(DataType* d1, DataType* d2);
    std::vector<std::tuple<std::string, DataType*>> getCaptures(std::vector<ASTNode*> statements, Scope* scope);
};

#endif