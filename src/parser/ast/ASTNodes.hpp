#ifndef ASTNODES_H
#define ASTNODES_H

#include <string>
#include <vector>

class ASTNode {
public:
    ASTNode();
    virtual std::string toString() = 0;
    virtual std::string toJS();
};

class DataType {
public:
    virtual std::string toString() = 0;
	virtual std::string toJS();
};

class Statement : public ASTNode {
public:
    virtual std::string toString() = 0;
	virtual std::string toJS();
};

class Expression : public ASTNode {
public:
    virtual std::string toString() = 0;
	virtual std::string toJS();
};

class Program : public ASTNode {
public:
    std::vector<ASTNode*> statements;
    Program(std::vector<ASTNode*> statements) : statements(statements) {}
    void print();
    std::string toString();
	std::string toJS();
};

class ListLiteral : public Expression {
public:
    std::vector<Expression*> elements;
    DataType* dataType;
    ListLiteral(std::vector<Expression*> elements, DataType* dataType);
    std::string toString();
	std::string toJS();
};

// Literal nodes
class IntegerLiteral : public Expression {
public:
    int value;
    IntegerLiteral(int value);
    std::string toString();
	std::string toJS();
};

class FloatLiteral : public Expression {
public:
    float value;
    FloatLiteral(float value);
    std::string toString();
	std::string toJS();
};

class StringLiteral : public Expression {
public:
    std::string value;
    StringLiteral(std::string value);
    std::string toString();
	std::string toJS();
};

class BooleanLiteral : public Expression {
public:
    bool value;
    BooleanLiteral(bool value);
    std::string toString();
	std::string toJS();
};

// Identifier node
class VariableDeclaration : public Statement {
public:
    std::string name;
    Expression* value;
    DataType* type;
    VariableDeclaration(std::string name, DataType* type, Expression* value);
    std::string toString();
	std::string toJS();
};

class IfStatement : public Statement {
public:
    Expression* condition;
    std::vector<ASTNode*> body;
    std::vector<ASTNode*> elseBody;
    IfStatement(Expression* condition, std::vector<ASTNode*> body, std::vector<ASTNode*> elseBlock);
    std::string toString();
	std::string toJS();
};

class VariableAssignment : public Statement {
public:
    std::string name;
    Expression* value;
    VariableAssignment(std::string name, Expression* value);
    std::string toString();
	std::string toJS();
};

class ReturnStatement : public Statement {
public:
    Expression* value;
    ReturnStatement(Expression* value);
    std::string toString();
	std::string toJS();
};

class FunctionParameter : public ASTNode {
public:
    std::string name;
    DataType* type;
    FunctionParameter(std::string name, DataType* type);
    std::string toString();
	std::string toJS();
};

class Function : public Expression {
public:
    std::string name;
    std::vector<FunctionParameter*> params;
    std::vector<ASTNode*> body;
    DataType* returnType;
    Function(std::vector<FunctionParameter*> params, DataType* returnType, std::vector<ASTNode*> body);
    std::string toString();
	std::string toJS();
};



class VariableAccess : public Expression {
public:
    std::string name;
    std::vector<Expression*> args;
    VariableAccess(std::string name, std::vector<Expression*> args);
    std::string toString();
	std::string toJS();
};

class BinaryExpression : public Expression {
public:
    Expression* left;
    Expression* right;
    std::string op;
    BinaryExpression(Expression* left, Expression* right, std::string op);
    std::string toString();
	std::string toJS();
};

class UnaryExpression : public Expression {
public:
    Expression* expr;
    std::string op;
    UnaryExpression(Expression* expr, std::string op);
    std::string toString();
	std::string toJS();
};



class IntegerType : public DataType {
public:
    IntegerType();
    std::string toString();
	std::string toJS();
};

class FloatType : public DataType {
public:
    FloatType();
    std::string toString();
	std::string toJS();
};

class FunctionType : public DataType {
public:
    FunctionType();
    std::string toString();
	std::string toJS();
};

class StringType : public DataType {
public:
    StringType();
    std::string toString();
	std::string toJS();
};

class BoolType : public DataType {
public:
    BoolType();
    std::string toString();
	std::string toJS();
};

class VoidType : public DataType {
public:
    VoidType();
    std::string toString();
	std::string toJS();
};

class ListType : public DataType {
public:
    DataType* subType;
    ListType(DataType* subType);
    std::string toString();
	std::string toJS();
};

#endif