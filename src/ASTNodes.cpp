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
        std::cout << "PRINTING: " << statement << std::endl;
        std::cout << statement->toString() << std::endl;
    }
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

VariableAssignment::VariableAssignment(std::string name, Expression* value) : name(name), value(value) {}

std::string VariableAssignment::toString() {
    return "VariableAssignment(name=" + name + ", value=" + value->toString() + ")";
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

VariableAccess::VariableAccess(std::string name) : name(name) {}

std::string VariableAccess::toString() {
    return "VariableAccess(name="+name+")";
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



