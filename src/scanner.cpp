#include "scanner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>

#include "errors.h"
#include "helpers.h"
#include "mymemory.h"
#include "mytypes.h"
#include "token.h"

struct Keyword
{
    const char* name;
    TokenType type;
    u32 len;
};

static constexpr Keyword keywords[]{
    Keyword{ "and", TokenType::AND, 3 },
    Keyword{ "class", TokenType::CLASS, 5 },
    Keyword{ "else", TokenType::ELSE, 4 },
    Keyword{ "false", TokenType::FALSE, 5 },
    Keyword{ "true", TokenType::TRUE, 4 },
    Keyword{ "func", TokenType::FUNC, 4 },
    Keyword{ "if", TokenType::IF, 2 },
    Keyword{ "nil", TokenType::NIL, 3 },
    Keyword{ "or", TokenType::OR, 2 },
    Keyword{ "print", TokenType::PRINT, 5 },
    Keyword{ "return", TokenType::RETURN, 6 },
    Keyword{ "super", TokenType::SUPER, 5 },
    Keyword{ "this", TokenType::THIS, 4 },
    Keyword{ "var", TokenType::VAR, 3 },
    Keyword{ "while", TokenType::WHILE, 5 },
};



static bool isAtAtEnd(const Scanner& scanner)
{
    return scanner.pos >= scanner.srcLen;
}

static u8 peek(const Scanner& scanner, i32 amount)
{
    if (scanner.pos + amount >= scanner.srcLen)
    {
        return '\0';
    }
    if (scanner.pos + amount < 0)
    {
        return '\0';
    }
    u8 c = scanner.src[scanner.pos + amount];
    return c;
}

static u8 peek(const Scanner& scanner)
{
    if (scanner.pos >= scanner.srcLen)
    {
        return '\0';
    }

    return peek(scanner, 0);
}

static u8 advance(Scanner& scanner)
{
    if (scanner.pos >= scanner.srcLen)
    {
        return '\0';
    }
    u8 c = peek(scanner);
    scanner.pos++;
    return c;
}

static void addNumberToken(Scanner& scanner)
{
    std::string s = std::string((const char*)&scanner.src[scanner.start], (size_t)(scanner.pos - scanner.start));

    // TODO fix this atof
    double d = atof(s.data());

    scanner.mem.tokens.emplace_back(Token{
        .value = {.doubleValue = d, .literalType = LiteralType_Double },
        .line = scanner.line,
        .type = TokenType::NUMBER,
    });
}

static void addIntToken(Scanner& scanner)
{
    std::string s = std::string((const char*)&scanner.src[scanner.start], (size_t)(scanner.pos - scanner.start));

    // TODO fix this atof
    i64 i = atoll(s.data());

    scanner.mem.tokens.emplace_back(Token{
        .value = {.value = i, .literalType = LiteralType_I64 },
        .line = scanner.line,
        .type = TokenType::INTEGER,
    });
}

static void addToken(Scanner& scanner, TokenType type)
{
    LiteralType literalType = type == TokenType::IDENTIFIER
        ? LiteralType_Identifier
        : LiteralType_None;
    u32 index = addString(
        scanner.mem, std::string((const char*)&scanner.src[scanner.start], (size_t)(scanner.pos - scanner.start)));
    scanner.mem.tokens.emplace_back(Token{
        //.lexMe = std::string((const char*)&scanner.src[scanner.start], (size_t)(scanner.pos - scanner.start)),
        .value = {.stringIndex = index, .literalType = literalType },
        .line = scanner.line,
        .type = type,
        });
}

static void handleStringLiteral(Scanner& scanner)
{
    while (peek(scanner) != '"' && !isAtAtEnd(scanner))
    {
        if (peek(scanner) == '\n')
        {
            scanner.line++;
        }
        advance(scanner);
    }

    if (isAtAtEnd(scanner))
    {
        reportError(scanner, "Unterminated string!", "");
        return;
    }
    u32 index = addString(
        scanner.mem, std::string((const char*)&scanner.src[scanner.start + 1], (size_t)(scanner.pos - scanner.start - 1)));

    scanner.mem.tokens.emplace_back(Token{
        // [start + 1, pos]
        //.lexMe = std::string((const char*)&scanner.src[scanner.start + 1], (size_t)(scanner.pos - scanner.start - 1)),
        .value = {.stringIndex = index, .literalType = LiteralType_String },
        .line = scanner.line,
        .type = TokenType::STRING,
        });

    advance(scanner);
}


static bool matchChar(Scanner& scanner, char c)
{
    if (scanner.pos >= scanner.srcLen)
    {
        return false;
    }
    if ((char)peek(scanner) != c)
    {
        return false;
    }
    advance(scanner);
    return true;
}


static void handleNumberString(Scanner& scanner)
{
    while (isdigit(peek(scanner)))
    {
        advance(scanner);
    }

    if (peek(scanner) == '.' && isdigit(peek(scanner, 1)))
    {
        advance(scanner);
        while (isdigit(peek(scanner)))
        {
            advance(scanner);
        }
        addNumberToken(scanner);
    }
    else
    {
        addIntToken(scanner);
    }

}

static void handleIdentifier(Scanner& scanner)
{
    while (isAlphaNumUnderscore((char)peek(scanner)))
    {
        advance(scanner);
    }
    i32 sz = scanner.pos - scanner.start;
    const char* identifier = (const char*)&scanner.src[scanner.start];
    for (const Keyword& word : keywords)
    {
        if (strncmp(identifier, word.name, word.len) == 0)
        {
            addToken(scanner, word.type);
            return;
        }
    }

    addToken(scanner, TokenType::IDENTIFIER);
}


static void scanToken(Scanner& scanner)
{
    char c = (char)advance(scanner);
    switch (c)
    {
    case '(': addToken(scanner, TokenType::LEFT_PAREN); break;
    case ')': addToken(scanner, TokenType::RIGHT_PAREN); break;
    case '{': addToken(scanner, TokenType::LEFT_BRACE); break;
    case '}': addToken(scanner, TokenType::RIGHT_BRACE); break;
    case ',': addToken(scanner, TokenType::COMMA); break;
    case '.': addToken(scanner, TokenType::DOT); break;
    case '-': addToken(scanner, TokenType::MINUS); break;
    case '+': addToken(scanner, TokenType::PLUS); break;
    case ';': addToken(scanner, TokenType::SEMICOLON); break;
    case '*': addToken(scanner, TokenType::STAR); break;
    case '\0': addToken(scanner, TokenType::END_OF_FILE); break;
        // Double char checks
    case '!': addToken(scanner, matchChar(scanner, '=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
    case '=': addToken(scanner, matchChar(scanner, '=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
    case '<': addToken(scanner, matchChar(scanner, '=') ? TokenType::LESSER_EQUAL : TokenType::LESSER); break;
    case '>': addToken(scanner, matchChar(scanner, '=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;

        // comments //
    case '/':
        if (matchChar(scanner, '/'))
        {
            while (peek(scanner) != '\n' && !isAtAtEnd(scanner))
            {
                advance(scanner);
            }
        }
        else
        {
            addToken(scanner, TokenType::SLASH);
        }
        break;

        // White spacessss...
    case '\t':
    case '\r':
    case ' ':
        // ignore white space
        break;
    case '\n':
        scanner.line++;
        break;

        // String literal
    case '"': handleStringLiteral(scanner); break;

    default:
        if (isdigit(c))
        {
            handleNumberString(scanner);
        }
        else if (isalpha(c))
        {
            handleIdentifier(scanner);
        }
        else
        {
            reportError(scanner, "Unexpected error on scanner.", "");
        }
        break;
    }

}

bool scanner_run(MyMemory& mem)
{
    Scanner scanner = {
        .mem = mem,
        .src = mem.scriptFileData.data(),
        .srcLen = (i32) mem.scriptFileData.size(),
        .pos = 0,
        .start = 0,
        .line = 1
    };

    while (!isAtAtEnd(scanner))
    {
        scanner.start = scanner.pos;
        scanToken(scanner);
    }

    scanner.mem.tokens.emplace_back(Token{
        //.lexMe = "",
        .line = scanner.line,
        .type = TokenType::END_OF_FILE
        });


    //for (const Token& token : scanner.mem.tokens)
    //{
    //    printToken(scanner.mem, token);
    //}

    return true;
}
