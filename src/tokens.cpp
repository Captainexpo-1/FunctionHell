#include "tokens.hpp"

std::vector<TOKENTYPE> token_priorities = {
    // Checked first
    WHITESPACE,
    NEWLINE,
    COMMENT,
    // ----- Keywords -----
    IF_KEYWORD,
    ELSE_KEYWORD,
    WITH_KEYWORD,
    AND_KEYWORD,
    OR_KEYWORD,
    VAR_KEYWORD,
    // ----- Types -----
    FLOAT_TYPE,
    INTEGER_TYPE,
    BOOL_TYPE,
    VOID_TYPE,
    NONE_TYPE,
    // ----- Literals -----
    BOOL,
    INTEGER,
    FLOAT,
    STRING,
    // ----- Special Characters -----
    LBRACE,
    RBRACE,
    LPAREN,
    RPAREN,
    LBRACKET,
    RBRACKET,
    SEMICOLON,
    COMMA,
    EQUAL_EQUAL,
    BANG_EQUAL,
    LESS_EQUAL,
    GREATER_EQUAL,
    LESS,
    GREATER,
    EQUAL,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    BANG,
    // ----- Other -----
    IDENTIFIER,

    // Checked last
};

std::map<int, std::string> token_regexes = {
    {NEWLINE, "\\n"},
    {WHITESPACE, "[ |\\t|\\r]+"},
    {IDENTIFIER, "[a-zA-Z_][a-zA-Z0-9_]*"},
    {INTEGER, "[0-9]+"},
    {FLOAT, "[0-9]+\\.[0-9]+"},
    {STRING, "\"[^\"]*\""},
    {BOOL, "true|false"},
    {INTEGER_TYPE, "int"},
    {FLOAT_TYPE, "float"},
    {STRING_TYPE, "string"},
    {BOOL_TYPE, "bool"},
    {VOID_TYPE, "void"},
    {NONE_TYPE, "none"},
    {IF_KEYWORD, "if"},
    {ELSE_KEYWORD, "else"},
    {LBRACE, "\\{"},
    {RBRACE, "\\}"},
    {LPAREN, "\\("},
    {RPAREN, "\\)"},
    {LBRACKET, "\\["},
    {RBRACKET, "\\]"},
    {SEMICOLON, ";"},
    {COMMA, ","},
    {EQUAL, "="},
    {PLUS, "\\+"},
    {MINUS, "-"},
    {STAR, "\\*"},
    {SLASH, "/"},
    {PERCENT, "%"},
    {BANG, "!"},
    {WITH_KEYWORD, "with"},
    {AND_KEYWORD, "and"},
    {OR_KEYWORD, "or"},
    {COMMENT, "//.*"},
    {EQUAL_EQUAL, "=="},
    {BANG_EQUAL, "!="},
    {LESS, "<"},
    {LESS_EQUAL, "<="},
    {GREATER, ">"},
    {GREATER_EQUAL, ">="},
    {VAR_KEYWORD, "var"},
};


std::map<int, std::string> token_strings = {
    {NEWLINE, "NEWLINE"},
    {END_OF_FILE, "END_OF_FILE"},
    {IDENTIFIER, "IDENTIFIER"},
    {INTEGER, "INTEGER"},
    {INTEGER_TYPE, "INTEGER_TYPE"},
    {FLOAT, "FLOAT"},
    {FLOAT_TYPE, "FLOAT_TYPE"},
    {STRING, "STRING"},
    {STRING_TYPE, "STRING_TYPE"},
    {BOOL, "BOOL"},
    {BOOL_TYPE, "BOOL_TYPE"},
    {VOID_TYPE, "VOID_TYPE"},
    {NONE_TYPE, "NONE_TYPE"},
    {IF_KEYWORD, "IF_KEYWORD"},
    {ELSE_KEYWORD, "ELSE_KEYWORD"},
    {LBRACE, "LBRACE"},
    {RBRACE, "RBRACE"},
    {LPAREN, "LPAREN"},
    {RPAREN, "RPAREN"},
    {LBRACKET, "LBRACKET"},
    {RBRACKET, "RBRACKET"},
    {SEMICOLON, "SEMICOLON"},
    {COMMA, "COMMA"},
    {EQUAL, "EQUAL"},
    {PLUS, "PLUS"},
    {MINUS, "MINUS"},
    {STAR, "STAR"},
    {SLASH, "SLASH"},
    {PERCENT, "PERCENT"},
    {BANG, "BANG"},
    {WITH_KEYWORD, "WITH_KEYWORD"},
    {AND_KEYWORD, "AND_KEYWORD"},
    {OR_KEYWORD, "OR_KEYWORD"},
    {COMMENT, "COMMENT"},
    {EQUAL_EQUAL, "EQUAL_EQUAL"},
    {BANG_EQUAL, "BANG_EQUAL"},
    {LESS, "LESS"},
    {LESS_EQUAL, "LESS_EQUAL"},
    {GREATER, "GREATER"},
    {GREATER_EQUAL, "GREATER_EQUAL"},
    {VAR_KEYWORD, "VAR_KEYWORD"},
};

TOKENTYPE DATA_TYPES[] = {
    INTEGER_TYPE, 
    FLOAT_TYPE, 
    STRING_TYPE, 
    BOOL_TYPE, 
    VOID_TYPE, 
    NONE_TYPE
};

std::vector<TOKENTYPE> ATOMS = {
    IDENTIFIER, 
    INTEGER, 
    FLOAT, 
    STRING, 
    BOOL, 
    LPAREN
};

Token::Token(TOKENTYPE type, std::string value, unsigned int line, unsigned int col)
    : type(type), value(value), line(line), col(col) {}

Token::Token()
    : type(END_OF_FILE), value(""), line(0), col(0) {}

std::string Token::toString() {
    return "Token(type: " + token_strings[type] + ", value: " + value + ", line: " + std::to_string(line) + ", col: " + std::to_string(col) + ")";
}