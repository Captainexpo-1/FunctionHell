#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <vector>
#include <iostream>
#include "../lexer/tokens/tokens.hpp"
#include "../lexer/lexer.hpp"
#include "./ast/ASTNodes.hpp"



class Parser {
public:
    Parser();
    Program* parse(std::vector<Token> tokens);
private:
    Token m_CurrentToken;
    int m_CurrentIndex;
    std::vector<Token> m_Tokens;
    void m_advance();
    Token m_peek();
    void m_skipWhitespace();
    Token m_eat(TOKENTYPE type);
    void m_mightEat(TOKENTYPE type);
    Token m_eatAny(std::vector<TOKENTYPE> types);
    Program* m_parseProgram();
    ASTNode* m_genericParse();
    Expression* m_parseExpression(int precedence = 0);
    ASTNode* m_parseNode();
    DataType* m_parseDataType();
    VariableDeclaration* m_parseVariableDeclaration();
    Expression* m_parseAtom();
    BinaryExpression* m_parseBinaryExpression(Expression left, int precedence);
    IfStatement* m_parseIfStatement();
    std::vector<ASTNode*> m_parseBlock();
    VariableAssignment* m_parseVariableAssignment();
    ASTNode* m_handleName();
    Function* m_parseFunctionLiteral();
    std::vector<FunctionParameter*> m_parseFunctionParams();
    std::vector<Expression*> m_parseFunctionCallArgs();
    ReturnStatement* m_parseReturnStatement();
    VariableAccess* m_parseVariableAccess();
    ListLiteral* m_parseListLiteral();
    Expression* m_handleDataTypeAtom();
};




#endif