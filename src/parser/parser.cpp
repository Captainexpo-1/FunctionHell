#include "parser.hpp"
#include "../errors/error.hpp"
#include <algorithm>

std::map<TOKENTYPE, int> ORDER_OF_OPERATIONS = {
    {PLUS, 10},
    {MINUS, 10},
    {STAR, 20},
    {SLASH, 20},
    {PERCENT, 20},
    {EQUAL, 5},
    {EQUAL_EQUAL, 5},
    {BANG_EQUAL, 5},
    {LESS, 5},
    {LESS_EQUAL, 5},
    {GREATER, 5},
    {GREATER_EQUAL, 5},
    {AND_KEYWORD, 3},
    {OR_KEYWORD, 2},
    {BANG, 1}
};


Parser::Parser() {
    m_CurrentIndex = 0;
}

Program* Parser::parse(std::vector<Token> tokens) {
    m_Tokens = tokens;
    m_CurrentToken = m_Tokens[m_CurrentIndex];
    return m_parseProgram();
}

void Parser::m_advance() {
    m_CurrentIndex += 1;
    if (m_CurrentIndex < (int)m_Tokens.size()) {
        m_CurrentToken = m_Tokens[m_CurrentIndex];
    }
}

void Parser::m_skipWhitespace() {
    while (m_CurrentToken.type == NEWLINE){
        m_eat(NEWLINE);
    }
}

Token Parser::m_eat(TOKENTYPE type) {
    if (m_CurrentToken.type == type) {
        Token eatenToken = m_CurrentToken;
        m_advance();
        return eatenToken;
    } else {
        langError("Unexpected token in EAT: " + m_CurrentToken.toString(), m_CurrentToken.line, m_CurrentToken.col);
    }
    return Token();
}



Token Parser::m_eatAny(std::vector<TOKENTYPE> types){
    for (int i = 0; i < ((int)sizeof(types))/((int)sizeof(TOKENTYPE)); i++) {
        TOKENTYPE type = types[i];
        if (m_CurrentToken.type == type) {
            Token eatenToken = m_CurrentToken;
            m_advance();
            return eatenToken;
        }
    }
    langError("Unexpected token in EATANY: " + m_CurrentToken.toString(), m_CurrentToken.line, m_CurrentToken.col);
    return Token();
}

Program* Parser::m_parseProgram() {
    std::vector<ASTNode*> statements;
    while (m_CurrentToken.type != END_OF_FILE) {
        ASTNode* node = m_parseNode();
        if (node != NULL) statements.push_back(node);
        if (m_CurrentToken.type == NEWLINE) m_eat(NEWLINE);
    }
    return new Program(statements);
}

std::vector<FunctionParameter*> Parser::m_parseFunctionParams() {
    std::vector<FunctionParameter*> params = {};
    while (m_CurrentToken.type != GREATER) {
        DataType* dataType = m_parseDataType(m_eatAny(DATA_TYPES));
        std::string name = m_eat(IDENTIFIER).value; 
        params.push_back(new FunctionParameter(name, dataType));
        if (m_CurrentToken.type != GREATER) m_eat(COMMA);
    }
    return params;
}

Function* Parser::m_parseFunctionLiteral() {
    DataType* dataType = m_parseDataType(m_eatAny(DATA_TYPES));
    m_eat(LESS);
    std::vector<FunctionParameter*> params = m_parseFunctionParams();
    m_eat(GREATER);
    std::vector<ASTNode*> body = m_parseBlock();
    return new Function(params, dataType, body);
}

Expression* Parser::m_parseExpression(int precedence) {
    Expression* left = m_parseAtom();
    while (ORDER_OF_OPERATIONS.find(m_CurrentToken.type) != ORDER_OF_OPERATIONS.end() && ORDER_OF_OPERATIONS[m_CurrentToken.type] >= precedence) {
        std::string op = m_CurrentToken.value;
        m_advance();
        Expression* right = m_parseExpression(ORDER_OF_OPERATIONS[m_CurrentToken.type]);
        left = new BinaryExpression(left, right, op);
    }
    return left;
}

ASTNode* Parser::m_parseNode() {
    if (m_CurrentToken.type == VAR_KEYWORD) {
        return m_parseVariableDeclaration();
    }
    else if(m_CurrentToken.type == IF_KEYWORD) {
        //std::cout << "PARSING IF STATEMENT" << std::endl;
        return m_parseIfStatement();
    }
    else if (m_CurrentToken.type == IDENTIFIER){
        return m_parseVariableAccess();
    }
    else if (std::find(ATOMS.begin(), ATOMS.end(), m_CurrentToken.type) != ATOMS.end()){
        return m_parseExpression();
    }
    else if (m_CurrentToken.type == RETURN_KEYWORD){
        return m_parseReturnStatement();
    }
    else if (m_CurrentToken.type == NEWLINE){
        m_eat(NEWLINE);
        return NULL;
    }
    else {
        langError("Unexpected token in parseNode: " + m_CurrentToken.toString(), m_CurrentToken.line, m_CurrentToken.col);
    }
    return nullptr;
}

VariableAssignment* Parser::m_parseVariableAssignment() {
    std::string name = m_eat(IDENTIFIER).value;
    m_eat(EQUAL);
    Expression* value = m_parseExpression();
    return new VariableAssignment(name, value);
}

DataType* Parser::m_parseDataType(Token data_type) {
    switch(data_type.type){
        case INTEGER_TYPE:
            return new IntegerType();
        case FLOAT_TYPE:
            return new FloatType();
        case STRING_TYPE:
            return new StringType();
        case BOOL_TYPE:
            return new BoolType();
        case VOID_TYPE:
            return new VoidType();
        default:
            langError("Unexpected token in PARSEDATATYPE: " + data_type.toString(), data_type.line, data_type.col);
    }
    return nullptr;
}

ReturnStatement* Parser::m_parseReturnStatement() {
    m_eat(RETURN_KEYWORD);
    Expression* value = m_parseExpression();
    return new ReturnStatement(value);
}

std::vector<Expression*> Parser::m_parseFunctionCallArgs() {
    std::vector<Expression*> args = {};
    m_eat(LPAREN);
    while (m_CurrentToken.type != RPAREN) {
        args.push_back(m_parseExpression());
        if (m_CurrentToken.type != RPAREN) m_eat(COMMA);
    }
    m_eat(RPAREN);
    return args;
}

VariableAccess* Parser::m_parseVariableAccess() {
    std::string name = m_eat(IDENTIFIER).value;
    std::vector<Expression*> args = {};
    //std::cout << "VAR ACCESS CURRENT TOKEN: " << m_CurrentToken.toString() << std::endl;
    if (m_CurrentToken.type == WITH_KEYWORD){
        m_eat(WITH_KEYWORD);
        args = m_parseFunctionCallArgs();
    }
    return new VariableAccess(name, args);
}

Expression* Parser::m_parseAtom() {
    if (m_CurrentToken.type == INTEGER){
        return new IntegerLiteral(std::stoi(m_eat(INTEGER).value));
    }
    else if (m_CurrentToken.type == FLOAT){
        return new FloatLiteral(std::stof(m_eat(FLOAT).value));
    }
    else if (m_CurrentToken.type == STRING){
        return new StringLiteral(m_eat(STRING).value);
    }
    else if (m_CurrentToken.type == BOOL){
        return new BooleanLiteral(m_eat(BOOL).value == "true");
    }
    else if (m_CurrentToken.type == IDENTIFIER){
        return m_parseVariableAccess();
    }
    else if (std::find(DATA_TYPES.begin(), DATA_TYPES.end(), m_CurrentToken.type) != DATA_TYPES.end()){
        return m_parseFunctionLiteral();
    }
    else if (m_CurrentToken.type == LPAREN){
        m_advance();
        Expression* expr = m_parseExpression();
        m_eat(RPAREN);
        return expr;
    }
    else if (m_CurrentToken.type == LESS){
        // This is a functon
    }
    else{
        langError("Unexpected token in PARSEATOM: " + m_CurrentToken.toString(), m_CurrentToken.line, m_CurrentToken.col);
    }
    return nullptr;
}

std::vector<ASTNode*> Parser::m_parseBlock() {
    std::vector<ASTNode*> statements;
    m_eat(LBRACE);
    m_skipWhitespace();
    while (m_CurrentToken.type != RBRACE) {
        ASTNode* node = m_parseNode();
        if (node != NULL) statements.push_back(node);
        m_skipWhitespace();
    }
    m_eat(RBRACE);
    return statements;
}

VariableDeclaration* Parser::m_parseVariableDeclaration() {
    m_eat(VAR_KEYWORD);
    DataType* data_type = m_parseDataType(m_eatAny(DATA_TYPES));
    std::string name = m_eat(IDENTIFIER).value;
    m_eat(EQUAL);
    Expression* value = m_parseExpression();
    return new VariableDeclaration(name, data_type, value);
}

IfStatement* Parser::m_parseIfStatement() {
    m_eat(IF_KEYWORD);
    m_eat(LPAREN);
    Expression* condition = m_parseExpression();
    m_eat(RPAREN);
    std::vector<ASTNode*> body = m_parseBlock();
    std::vector<ASTNode*> elseBody;
    m_skipWhitespace();
    //std::cout << "END OF IF: " << m_CurrentToken.toString() << std::endl;
    if (m_CurrentToken.type == ELSE_KEYWORD) {
        m_eat(ELSE_KEYWORD);
        if (m_CurrentToken.type == IF_KEYWORD){
            elseBody = {
                m_parseIfStatement()
            };
        }else{
            elseBody = m_parseBlock();
        }
    }
    return new IfStatement(condition, body, elseBody);
}