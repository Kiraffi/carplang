#pragma once

#include "mytypes.h"
#include <string>
#include <vector>

enum class TokenType: u8
{
    //Single character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESSER, LESSER_EQUAL,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    CLASS, SUPER,
    AND, OR,
    ELSE, FUNC, FOR, IF, NIL, THIS, WHILE,
    PRINT, RETURN,
    TRUE, FALSE, VAR,

    // End of file
    END_OF_FILE,
};

static const char* TOKEN_NAMES[] = {
    //Single character tokens
    "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE",
    "COMMA", "DOT", "MINUS", "PLUS", "SEMICOLON", "SLASH", "STAR",

    // One or two character tokens.
    "BANG", "BANG_EQUAL",
    "EQUAL", "EQUAL_EQUAL",
    "GREATER", "GREATER_EQUAL",
    "LESSER", "LESSER_EQUAL",

    // Literals
    "IDENTIFIER", "STRING", "NUMBER",

    // Keywords
    "CLASS", "SUPER",
    "AND", "OR",
    "ELSE", "FUNC", "FOR", "IF", "NIL", "THIS", "WHILE",
    "PRINT", "RETURN",
    "TRUE", "FALSE", "VAR",

    // End of file
    "END_OF_FILE",
};

struct Token
{
    // ?
    std::string lexMe;
    // object? object;
    i32 line;
    TokenType type;

};


void printToken(const Token& token);

