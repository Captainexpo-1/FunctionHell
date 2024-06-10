#include "parser.hpp"


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

void Parser::m_error(std::string message, int line, int col) {
    std::cerr << message << " at line " << line << ", column " << col << std::endl;
    exit(1);
}

Program* Parser::parse(std::vector<Token> tokens) {
    m_Tokens = tokens;
    m_CurrentToken = m_Tokens[m_CurrentIndex];
    return m_parseProgram();
}

void Parser::m_advance() {
    m_CurrentIndex += 1;
    if (m_CurrentIndex < m_Tokens.size()) {
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
        m_error("Unexpected token: " + m_CurrentToken.toString(), m_CurrentToken.line, m_CurrentToken.col);
    }
}



Token Parser::m_eatAny(TOKENTYPE types[]){
    for (int i = 0; i < sizeof(types)/sizeof(TOKENTYPE); i++) {
        TOKENTYPE type = types[i];
        if (m_CurrentToken.type == type) {
            Token eatenToken = m_CurrentToken;
            m_advance();
            return eatenToken;
        }
    }
    m_error("Unexpected token: " + m_CurrentToken.toString(), m_CurrentToken.line, m_CurrentToken.col);
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

Expression* Parser::m_parseExpression(int precedence) {
    if (precedence == NULL){
        precedence = 0;
    }
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
        std::cout << "PARSING IF STATEMENT" << std::endl;
        return m_parseIfStatement();
    }
    else if (m_CurrentToken.type == IDENTIFIER) {
        return m_parseVariableAssignment();
    }
    else if (std::find(ATOMS.begin(), ATOMS.end(), m_CurrentToken.type) != ATOMS.end()){
        return m_parseExpression();
    }
    else if (m_CurrentToken.type == NEWLINE){
        m_eat(NEWLINE);
        return NULL;
    }
    else {
        m_error("Unexpected token in parseNode: " + m_CurrentToken.toString(), m_CurrentToken.line, m_CurrentToken.col);
    }

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
            m_error("Unexpected token: " + data_type.toString(), data_type.line, data_type.col);
    }
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
        return new VariableAccess(m_eat(IDENTIFIER).value);
    }
    else if (m_CurrentToken.type == LPAREN){
        m_advance();
        Expression* expr = m_parseExpression();
        m_eat(RPAREN);
        return expr;
    }
    else{
        m_error("Unexpected token: " + m_CurrentToken.toString(), m_CurrentToken.line, m_CurrentToken.col);
    }
}

std::vector<ASTNode*> Parser::m_parseBlock() {
    std::vector<ASTNode*> statements;
    m_eat(LBRACE);
    while (m_CurrentToken.type != RBRACE) {
        ASTNode* node = m_parseNode();
        if (node != NULL) statements.push_back(node);
        if (m_CurrentToken.type == NEWLINE) m_eat(NEWLINE);
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
    std::cout << "END OF IF: " << m_CurrentToken.toString() << std::endl;
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