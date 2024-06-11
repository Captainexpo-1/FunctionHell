#include "ASTNodes.hpp"
#include <iostream>

ASTNode::ASTNode() {}

std::string ASTNode::toString() {
    return "ASTNode";
}


std::string Statement::toString() {
    return "Statement";
}
std::string Expression::toString() {
    return "Expression";
}

std::string Program::toString() {
    std::string result = "";
    for (ASTNode* statement : statements) {
        result += statement->toString() + "\n";
    }
    return result;
}

void Program::print() {
    for (ASTNode* statement : statements) {
        std::cout << statement->toString() << std::endl;
    }
}

ListLiteral::ListLiteral(std::vector<Expression*> elements, DataType* dataType) : elements(elements), dataType(dataType) {};

std::string ListLiteral::toString(){
    std::string result = "ListLiteral(elements=(";
    for (Expression* e: elements){
        result += e->toString() + ", ";
    }
    return result + "))";
}

IntegerLiteral::IntegerLiteral(int value) : value(value) {}

std::string IntegerLiteral::toString() {
    return std::to_string(value);
}

FloatLiteral::FloatLiteral(float value) : value(value) {}

std::string FloatLiteral::toString() {
    return std::to_string(value);
}

StringLiteral::StringLiteral(std::string value) : value(value) {}

std::string StringLiteral::toString() {
    return value;
}

BooleanLiteral::BooleanLiteral(bool value) : value(value) {}

std::string BooleanLiteral::toString() {
    return value ? "true" : "false";
}

VariableDeclaration::VariableDeclaration(std::string name, DataType* type, Expression* value) : name(name), type(type), value(value) {}

std::string VariableDeclaration::toString() {
    return "VariableDeclaration(name=" + name + ", type=" + type->toString() + ", value=" + value->toString() + ")";
}

Function::Function(std::vector<FunctionParameter*> params, DataType* returnType, std::vector<ASTNode*> body) : params(params), returnType(returnType), body(body) {}

std::string Function::toString() {
    std::string o = "Function(params=(";
    for(FunctionParameter* param: params){
        o += param->toString() + ", ";
    }
    o += "), returnType=" + returnType->toString() + ", body=(";
    for(ASTNode* node: body){
        o += node->toString() + ", ";
    }
    o += "))";
    return o;
}

FunctionParameter::FunctionParameter(std::string name, DataType* type) : name(name), type(type) {}

std::string FunctionParameter::toString() {
    return "FunctionParameter(name=" + name + ", type=" + type->toString() + ")";
}

VariableAssignment::VariableAssignment(std::string name, Expression* value) : name(name), value(value) {}

std::string VariableAssignment::toString() {
    return "VariableAssignment(name=" + name + ", value=" + value->toString() + ")";
}

ReturnStatement::ReturnStatement(Expression* value) : value(value) {}

std::string ReturnStatement::toString() {
    return "ReturnStatement(value=" + value->toString() + ")";
}

IfStatement::IfStatement(Expression* condition, std::vector<ASTNode*> body, std::vector<ASTNode*> elseBody) : condition(condition), body(body), elseBody(elseBody) {}

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


std::string DataType::toString() {
    return "DataType";
}

IntegerType::IntegerType() {}

std::string IntegerType::toString() {
    return "int";
}

FloatType::FloatType() {}

std::string FloatType::toString() {
    return "float";
}

StringType::StringType() {}

std::string StringType::toString() {
    return "string";
}

BoolType::BoolType() {}

std::string BoolType::toString() {
    return "bool";
}

VoidType::VoidType() {}

std::string VoidType::toString() {
    return "void";
}

ListType::ListType(DataType* subType) : subType(subType) {};

std::string ListType::toString(){
    return "list(" + subType->toString() + ")";
}