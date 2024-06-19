#include "ASTNodes.hpp"
#include <iostream>

ASTNode::ASTNode() {}

std::string ASTNode::toString() {
    return "ASTNode";
}

std::string ASTNode::toJS() {
    return "null";
}

std::string Statement::toString() {
    return "Statement";
}

std::string Statement::toJS() {
    return "null";
}

std::string Expression::toString() {
    return "Expression";
}

std::string Expression::toJS() {
    return "null";
}

std::string Program::toString() {
    std::string result = "";
    for (ASTNode* statement : statements) {
        result += statement->toString() + "\n";
    }
    return result;
}

std::string Program::toJS() {
    std::string result = "";
    for (ASTNode* statement : statements) {
        result += statement->toJS() + ";";
    }
    return result;
}

void Program::print() {
    for (ASTNode* statement : statements) {
        std::cout << statement->toString() << std::endl;
    }
}

ListLiteral::ListLiteral(std::vector<Expression*> elements, DataType* dataType) : elements(elements), dataType(dataType) {};

std::string ListLiteral::toJS(){
    std::string result = "[";
    for (Expression* e: elements){
        result += e->toJS() + ", ";
    }
    return result + "]";
}

std::string ListLiteral::toString(){
    std::string result = "ListLiteral(elements=(";
    for (Expression* e: elements){
        result += e->toString() + ", ";
    }
    return result + ", type=" + dataType->toString() +"))";
}

IntegerLiteral::IntegerLiteral(int value) : value(value) {}

std::string IntegerLiteral::toJS() {
    return std::to_string(value);
}

std::string IntegerLiteral::toString() {
    return std::to_string(value);
}

FloatLiteral::FloatLiteral(float value) : value(value) {}

std::string FloatLiteral::toJS() {
    return std::to_string(value);
}

std::string FloatLiteral::toString() {
    return std::to_string(value);
}

StringLiteral::StringLiteral(std::string value) {
    this->value = value;
    if(this->value[0] == '\"'){
        this->value = this->value.substr(1, this->value.length());
    }
    if(this->value[this->value.length()-1] == '\"'){
        this->value = this->value.substr(0, this->value.length()-1);
    }
}   

std::string StringLiteral::toJS() {
    return "\"" + value + "\"";
}

std::string StringLiteral::toString() {
    return value;
}

BooleanLiteral::BooleanLiteral(bool value) : value(value) {}

std::string BooleanLiteral::toJS() {
    return this->toString();
}

std::string BooleanLiteral::toString() {
    return value ? "true" : "false";
}

VariableDeclaration::VariableDeclaration(std::string name, DataType* type, Expression* value) : name(name), type(type), value(value) {}

std::string VariableDeclaration::toJS() {
    //return "let " + name + ": () => " + type->toJS() + " = "  + value->toJS();
    return "let " + name + " = "  + value->toJS() + ";if (typeof " + name + " !== 'function') varErr();";
}

std::string VariableDeclaration::toString() {
    return "VariableDeclaration(name=" + name + ", type=" + type->toString() + ", value=" + value->toString() + ")";
}

Function::Function(std::vector<FunctionParameter*> params, DataType* returnType, std::vector<ASTNode*> body) : params(params), returnType(returnType), body(body) {}

std::string Function::toJS(){
    // Functions are always lambda functions
    std::string o = "";
    o += "(";
    for(FunctionParameter* param: params){
        o += param->toJS() + ", ";
    }
    o += ") => {";
    for(ASTNode* node: body){
        o += node->toJS() + ";";
    }
    o += "}";
    return o;

}

std::string Function::toString() {
    std::string o = "(";
    // To define functions in TypeScript, you can use the following syntax:
    // (args): type => { body }
    for(FunctionParameter* param: params){
        o += param->toString() + ",";
    }
    o += "): " + returnType->toString() + " => {";
    for(ASTNode* node: body){
        o += node->toString() + ",";
    }
    o += "}";

    return o;
}

FunctionParameter::FunctionParameter(std::string name, DataType* type) : name(name), type(type) {}

std::string FunctionParameter::toJS() {
    return name;
}

std::string FunctionParameter::toString() {
    return "FunctionParameter(name=" + name + ", type=" + type->toString() + ")";
}

VariableAssignment::VariableAssignment(std::string name, Expression* value) : name(name), value(value) {}

std::string VariableAssignment::toJS() {
    return name + " = " + value->toJS() + ";if (typeof " + name + " !== 'function') varErr();";
}

std::string VariableAssignment::toString() {
    return "VariableAssignment(name=" + name + ", value=" + value->toString() + ")";
}

ReturnStatement::ReturnStatement(Expression* value) : value(value) {}

std::string ReturnStatement::toJS() {
    return "return " + value->toJS();
}

std::string ReturnStatement::toString() {
    return "ReturnStatement(value=" + value->toString() + ")";
}

IfStatement::IfStatement(Expression* condition, std::vector<ASTNode*> body, std::vector<ASTNode*> elseBody) : condition(condition), body(body), elseBody(elseBody) {}

std::string IfStatement::toJS() {
    std::string o = "if(" + condition->toJS() + ") {";
    for(ASTNode* node: body){
        o += node->toJS() + ";";
    }
    if (elseBody.size() == 0){
        o += "}";
        return o;
    }
    o += "} else {";
    for(ASTNode* node: elseBody){
        o += node->toJS() + ";";
    }
    o += "}";
    return o;
}

std::string IfStatement::toString() {
    std::string o = "IfStatement(condition=" + condition->toString() + ", body=(";
    for(ASTNode* node: body){
        o += node->toString() + ", ";
    }
    o += ") elsebody=(";
    for(ASTNode* node: elseBody){
        o += node->toString() + ", ";
    }
    o += "))";
    return o;
}

VariableAccess::VariableAccess(std::string name, std::vector<Expression*> args) : name(name), args(args) {}

std::string VariableAccess::toJS() {
    std::string o = name + "(";
    for(Expression* arg: args){
        o += arg->toJS() + ", ";
    }
    o += ")";
    return o;
}

std::string VariableAccess::toString() {
    std::string o = "VariableAccess(name=" + name + ", args=(";
    for(Expression* arg: args){
        o += arg->toString() + ", ";
    }
    o += "))";
    return o;
}

BinaryExpression::BinaryExpression(Expression* left, Expression* right, std::string op) : left(left), op(op), right(right) {}

std::string BinaryExpression::toString() {
    return "BinaryExpression(" + left->toString() + " " + op + " " + right->toString() + ")";
}

std::string BinaryExpression::toJS() {
    bool useParens = true;
    if (op[0] == '=' || op == "!=" || op[0] == '>' || op[0] == '<')
        useParens = false;
    return (useParens ? "(" : "") + left->toJS() + " " + op + " " + right->toJS() + (useParens ? ")" : "");
}

std::string DataType::toString() {
    return "DataType";
}

std::string DataType::toJS() {
    return "null";
}

IntegerType::IntegerType() {}
std::string IntegerType::toJS(){
    return "number";
}
std::string IntegerType::toString() {
    return "string";
}


FloatType::FloatType() {}
std::string FloatType::toJS(){
    return "number";
}
std::string FloatType::toString() {
    return "float";
}

FunctionType::FunctionType() {}
std::string FunctionType::toJS(){
    return "Function";
}

std::string FunctionType::toString(){
    return "function";
}
StringType::StringType() {}

std::string StringType::toJS(){
    return "string";
}

std::string StringType::toString() {
    return "string";
}

BoolType::BoolType() {}

std::string BoolType::toJS(){
    return "boolean";
}

std::string BoolType::toString() {
    return "bool";
}

VoidType::VoidType() {}

std::string VoidType::toJS(){
    return "void";
}

std::string VoidType::toString() {
    return "void";
}

ListType::ListType(DataType* subType) : subType(subType) {};

std::string ListType::toJS(){
    return "any";
}

std::string ListType::toString(){
    return "list(" + subType->toString() + ")";
}