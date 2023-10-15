#include "token.h"

#include "mymemory.h"
#include "token.h"

#include <string>

std::string getTokenValueAsString(const MyMemory& mem, const Token& token)
{

    switch(token.type)
    {
        case TokenType::INTEGER:
            return std::to_string(token.value.value);
        case TokenType::NUMBER:
            return std::to_string(token.value.doubleValue);
        default:
            return mem.strings[token.value.stringIndex];
    }

}

void printToken(const MyMemory& mem, const Token& token)
{
    const char* tokenTypeName = TOKEN_NAMES[(i32)token.type];
    printf("Token type: %s, %s, literal?\n", tokenTypeName, getTokenValueAsString(mem, token).data());
}
