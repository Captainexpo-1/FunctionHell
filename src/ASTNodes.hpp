#ifndef ASTNODES_H
#define ASTNODES_H

#include <string>
#include <vector>

class ASTNode {
public:
    ASTNode();
    virtual std::string toString() = 0;
};

class DataType {
public:
    virtual std::string toString() = 0;
};

class Statement : public ASTNode {
public:
    virtual std::string toString() = 0;
};

class Expression : public ASTNode {
public:
    virtual std::string toString() = 0;
};

class Program : public ASTNode {
public:
    std::vector<ASTNode*> statements;
    Program(std::vector<ASTNode*> statements) : statements(statements) {}
    void print();
    std::string toString();
};

// Literal nodes
class IntegerLiteral : public Expression {
public:
    int value;
    IntegerLiteral(int value);
    std::string toString();
};

class FloatLiteral : public Expression {
public:
    float value;
    FloatLiteral(float value);
    std::string toString();
};

class StringLiteral : public Expression {
public:
    std::string value;
    StringLiteral(std::string value);
    std::string toString();
};

class BooleanLiteral : public Expression {
public:
    bool value;
    BooleanLiteral(bool value);
    std::string toString();
};

// Identifier node
class VariableDeclaration : public Statement {
public:
    std::string name;
    Expression* value;
    DataType* type;
    VariableDeclaration(std::string name, DataType* type, Expression* value);
    std::string toString();
};

class IfStatement : public Statement {
public:
    Expression* condition;
    std::vector<ASTNode*> body;
    std::vector<ASTNode*> elseBody;
    IfStatement(Expression* condition, std::vector<ASTNode*> body, std::vector<ASTNode*> elseBlock);
    std::string toString();
};

class VariableAssignment : public Statement {
public:
    std::string name;
    Expression* value;
    VariableAssignment(std::string name, Expression* value);
    std::string toString();
};

class ReturnStatement : public Statement {
public:
    Expression* value;
    ReturnStatement(Expression* value);
    std::string toString();
};

class FunctionParameter : public ASTNode {
public:
    std::string name;
    DataType* type;
    FunctionParameter(std::string name, DataType* type);
    std::string toString();
};

class Function : public Expression {
public:
    std::string name;
    std::vector<FunctionParameter*> params;
    std::vector<ASTNode*> body;
    DataType* returnType;
    Function(std::vector<FunctionParameter*> params, DataType* returnType, std::vector<ASTNode*> body);
    std::string toString();
};



class VariableAccess : public Expression {
public:
    std::string name;
    std::vector<Expression*> args;
    VariableAccess(std::string name, std::vector<Expression*> args);
    std::string toString();
};

class BinaryExpression : public Expression {
public:
    Expression* left;
    Expression* right;
    std::string op;
    BinaryExpression(Expression* left, Expression* right, std::string op);
    std::string toString();
};

class UnaryExpression : public Expression {
public:
    Expression* expr;
    std::string op;
    UnaryExpression(Expression* expr, std::string op);
    std::string toString();
};



class IntegerType : public DataType {
public:
    IntegerType();
    std::string toString();
};

class FloatType : public DataType {
public:
    FloatType();
    std::string toString();
};

class StringType : public DataType {
public:
    StringType();
    std::string toString();
};

class BoolType : public DataType {
public:
    BoolType();
    std::string toString();
};

class VoidType : public DataType {
public:
    VoidType();
    std::string toString();
};

#endif