#include "transpiler.hpp"

Transpiler::Transpiler() {
    m_output = "";
}

void Transpiler::m_addStd(){
    m_output += 
    // functions
    "const __fs__ = require('fs');\n"
    "var fileRead = (path) => __fs__.readFileSync(path, 'utf8');\n"
    "var fileWrite = (path, data) => { try { __fs__.writeFileSync('output.txt', data, 'utf8'); } catch (err) { console.error('Error writing to file:', err); }};\n"
    "var fileAppend = (path, data) => { try { __fs__.appendFileSync('output.txt', data, 'utf8'); } catch (err) { console.error('Error writing to file:', err); }};\n"
    "var varErr = () => console.error('Variable is not a function');\n"
    "var log = (...args) => { let x = []; for(let i = 0; i < args.length; i++) { x.push(args[i]()) } console.log(...x)}\n"
    "var at = (x, index) => x()[index];"
    "var sin = (x) => Math.sin(x);\n" 
    "var cos = (x) => Math.cos(x);\n" 
    "var tan = (x) => Math.tan(x);\n"
    "var abs = (x) => x() > 0 ? x() : -x();\n"
    "var parseint = (x) => parseInt(x());\n"
    "var parsefloat = (x) => parseFloat(x());\n"
    "var len = (x) => x().length;\n"
    "var str = (x) => x().toString();\n"
    // constants
    "var PI = () => 3.14159265;\n";
}


std::string Transpiler::transpile(std::vector<ASTNode*> statements) {
    m_addStd();
    for (ASTNode* statement : statements) {
        m_output += m_genericTranspile(statement);
        // Make sure there is a semicolon at the end of the statement
        if ( m_output[m_output.size()-1] != ';') {
            m_output += ';';
        }
        //m_output += "\n";
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
    return statement->toJS();
}

std::string Transpiler::m_transpileExpression(Expression* expression) {
    return expression->toJS();
}

