#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>

#include "errors.h"
#include "tokens.h"
#include "types.h"

using f32 = float;
using f64 = double;

struct Keyword
{
    const char* name;
    TokenType type;
};

static constexpr Keyword keywords[]{
    Keyword{ "and", TokenType::AND },
    Keyword{ "class", TokenType::CLASS },
    Keyword{ "else", TokenType::ELSE },
    Keyword{ "false", TokenType::FALSE },
    Keyword{ "true", TokenType::TRUE },
    Keyword{ "func", TokenType::FUNC },
    Keyword{ "if", TokenType::IF },
    Keyword{ "nil", TokenType::NIL },
    Keyword{ "or", TokenType::OR },
    Keyword{ "print", TokenType::PRINT },
    Keyword{ "return", TokenType::RETURN },
    Keyword{ "super", TokenType::SUPER },
    Keyword{ "this", TokenType::THIS },
    Keyword{ "var", TokenType::VAR },
    Keyword{ "while", TokenType::WHILE },
};


static bool isAlphaNumUnderscore(char c)
{
    return isalnum(c) || c == '_';
}

static bool isAtAtEnd(const Scanner& scanner)
{
    return scanner.pos >= scanner.srcLen;
}

static u8 peek(const Scanner& scanner, i32 amount)
{
    if(scanner.pos + amount >= scanner.srcLen)
    {
        return '\0';
    }
    if(scanner.pos + amount < 0)
    {
        return '\0';
    }
    u8 c = scanner.src[scanner.pos + amount];
    return c;
}

static u8 peek(const Scanner& scanner)
{
    if(scanner.pos >= scanner.srcLen)
    {
        return '\0';
    }

    return peek(scanner, 0);
}

static u8 advance(Scanner& scanner)
{
    if(scanner.pos >= scanner.srcLen)
    {
        return '\0';
    }
    u8 c = peek(scanner);
    scanner.pos++;
    return c;
}


static void addToken(Scanner& scanner, TokenType type)
{
    scanner.tokens.emplace_back(Token{
        .lexMe = std::string((const char*)&scanner.src[scanner.start], (size_t)(scanner.pos - scanner.start)),
        .line = scanner.line,
        .type = type,
    });
}

static void handleStringLiteral(Scanner& scanner)
{
    while(peek(scanner) != '"' && !isAtAtEnd(scanner))
    {
        if(peek(scanner) == '\n')
        {
            scanner.line++;
        }
        advance(scanner);
    }

    if(isAtAtEnd(scanner))
    {
        reportError(scanner, "Unterminated string!", "");
        return;
    }
    scanner.tokens.emplace_back(Token{
        // [start + 1, pos]
        .lexMe = std::string((const char*)&scanner.src[scanner.start + 1], (size_t)(scanner.pos - scanner.start - 1)),
        .line = scanner.line,
        .type = TokenType::STRING,
    });

    advance(scanner);
}


static bool matchChar(Scanner& scanner, char c)
{
    if(scanner.pos >= scanner.srcLen)
    {
        return false;
    }
    if((char)peek(scanner) != c)
    {
        return false;
    }
    advance(scanner);
    return true;
}


static void handleNumberString(Scanner& scanner)
{
    while(isdigit(peek(scanner)))
    {
        advance(scanner);
    }

    if(peek(scanner) == '.' && isdigit(peek(scanner, 1)))
    {
        advance(scanner);
        while(isdigit(peek(scanner)))
        {
            advance(scanner);
        }
    }

    addToken(scanner, TokenType::NUMBER);
}

static void handleIdentifier(Scanner& scanner)
{
    while(isAlphaNumUnderscore((char)peek(scanner)))
    {
        advance(scanner);
    }
    i32 sz = scanner.pos - scanner.start;
    const char* identifier = (const char*)&scanner.src[scanner.start];
    for(const Keyword& word : keywords)
    {
        if(strncmp(identifier, word.name, sz) == 0)
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
    switch(c)
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

        // Double char checks
        case '!': addToken(scanner, matchChar(scanner, '=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': addToken(scanner, matchChar(scanner, '=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
        case '<': addToken(scanner, matchChar(scanner, '=') ? TokenType::LESSER_EQUAL : TokenType::LESSER); break;
        case '>': addToken(scanner, matchChar(scanner, '=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;

        // comments //
        case '/':
            if(matchChar(scanner, '/'))
            {
                while(peek(scanner) != '\n' && !isAtAtEnd(scanner))
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
            if(isdigit(c))
            {
                handleNumberString(scanner);
            }
            else if(isalpha(c))
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

static bool runProject(const std::vector<u8>& carpData)
{
    Scanner scanner{
        .src = carpData.data(),
        .srcLen = (i32)carpData.size(),
        .pos = 0,
        .start = 0,
        .line = 1,
    };

    while(!isAtAtEnd(scanner))
    {
        scanner.start = scanner.pos;
        scanToken(scanner);
    }

    scanner.tokens.emplace_back(Token{
        .lexMe = "",
        .line = scanner.line,
        .type = TokenType::END_OF_FILE
    });


    for(const Token& token : scanner.tokens)
    {
        printToken(token);
    }

    return true;
}


static bool runFile(const char* filename)
{
    printf("Filename: %s\n", filename);

    if(filename == nullptr)
    {
        LOG_ERROR("Filename is nullptr");
        return false;
    }

    FILE* file = fopen(filename, "rb");
    if(file == nullptr)
    {
        LOG_ERROR("Failed to open file.");
        return false;
    }

    std::vector<u8> data;

    fseek(file, 0L, SEEK_END);
    size_t sz = ftell(file);
    fseek(file, 0L, SEEK_SET);

    data.resize(sz);
    fread(data.data(), 1, sz, file);
    fclose(file);

    if(!runProject(data))
    {
        printf("Some failure in: %s\n", filename);
    }
    printf("%s\n", data.data());

    return true;
}



static void runPrompt()
{
}

int main(int argc, const char** argv)
{
    if(argc > 3)
    {
        printf("Usage: carp [script]\n");
        return 64;
    }
    else if(argc == 2)
    {
        if(!runFile(argv[1]))
        {
            printf("Failed to run file: %s\n", argv[1]);
        }
    }
    else
    {
        const char* filename = "progs/print.carp";
        if(!runFile(filename))
        {
            printf("Failed to run file: %s\n", filename);
        }

        //runPrompt();
    }

    return 0;
}
