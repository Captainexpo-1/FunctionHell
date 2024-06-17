#include <transpiler.hpp>

Transpiler::Transpiler() {
    m_output = "";
}


std::string Transpiler::transpile(std::vector<ASTNode*> statements) {
    for (ASTNode* statement : statements) {
        m_output += m_genericTranspile(statement);
    }
    return m_output;
}