#include "token.h"

void printToken(const Token& token)
{
    const char* tokenTypeName = TOKEN_NAMES[(i32)token.type];
    printf("Token type: %s, %s, literal?\n", tokenTypeName, token.lexMe.data());
}
