#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <vector>
#include <iostream>
#include "tokens.hpp"
#include "lexer.hpp"
#include "ASTNodes.hpp"



class Parser {
public:
    Parser();
    Program* parse(std::vector<Token> tokens);
private:
    Token m_CurrentToken;
    int m_CurrentIndex;
    std::vector<Token> m_Tokens;
    void m_error(std::string message, int line, int col);
    void m_advance();
    void m_skipWhitespace();
    Token m_eat(TOKENTYPE type);
    Token m_eatAny(TOKENTYPE types[]);
    Program* m_parseProgram();
    ASTNode* m_genericParse();
    Expression* m_parseExpression(int precedence = 0);
    ASTNode* m_parseNode();
    DataType* m_parseDataType(Token data_type);
    VariableDeclaration* m_parseVariableDeclaration();
    Expression* m_parseAtom();
    BinaryExpression* m_parseBinaryExpression(Expression left, int precedence);
    IfStatement* m_parseIfStatement();
    std::vector<ASTNode*> m_parseBlock();
    VariableAssignment* m_parseVariableAssignment();

};




#endif