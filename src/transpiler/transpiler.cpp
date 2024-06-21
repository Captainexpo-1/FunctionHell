#include "transpiler.hpp"

Transpiler::Transpiler() {
    m_output = "";
}

void Transpiler::m_addStd(){
    m_output += 
    // functions
    "declare function require(name:string);"
    "const __fs__ = require('fs');"
    "var fileRead=(path)=>__fs__.readFileSync(path, 'utf8');"
    "var fileWrite=(path, data)=>{try { __fs__.writeFileSync('output.txt', data, 'utf8'); console.log('File has been written'); } catch (err) { console.error('Error writing to file:', err); }};"
    "var varErr=()=>console.error('Variable is not a function');"
    "var log=(...args)=>console.log(...args);"
    "var at: () => any =(x, index)=>{x()[index];};"
    "var sin: () => number =(x)=>Math.sin(x);" 
    "var cos: () => number =(x)=>Math.cos(x);" 
    "var tan: () => number =(x)=>Math.tan(x);"
    "var abs: () => number =(x)=>x() > 0 ? x() : -x();"
    "var parseint: () => number =(x)=>parseInt(x());"
    "var parsefloat: () => number =(x)=>parseFloat(x());"
    "var len: () => number =(x)=>x().length;"
    "var str: () => string =(x)=>x().toString();"
    // constants
    "var PI:() => number =()=>3.14159265;";
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

