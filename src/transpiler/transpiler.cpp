#include "transpiler.hpp"

Transpiler::Transpiler() {
    m_output = "";
}


std::string Transpiler::transpile(std::vector<ASTNode*> statements) {
    for (ASTNode* statement : statements) {
        m_output += m_genericTranspile(statement);
    }
    return m_output;
}

std::string Transpiler::m_genericTranspile(ASTNode* node) {
    // Check if node is expression or statement
    if (dynamic_cast<Expression*>(node) != nullptr) {
        return m_transpileExpression(dynamic_cast<Expression*>(node));
    } else if (dynamic_cast<Statement*>(node) != nullptr) {
        return m_tanspileStatement(dynamic_cast<Statement*>(node));
    } else {
        return "Unknown node type";
    }
}


std::string Transpiler::m_tanspileStatement(Statement* statement) {
    return statement->toCPP();
}

std::string Transpiler::m_transpileExpression(Expression* expression) {
    return expression->toCPP();
}

