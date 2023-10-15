#include "errors.h"

#include "scanner.h"
#include "token.h"

void reportError(i32 line, const std::string& message, const std::string& where)
{
    printf("[line: %i] Error %s: %s\n", line, where.data(), message.data());
}
void reportError(Scanner& scanner, const std::string& message, const std::string& where)
{
    scanner.hasErrors = true;
    reportError(scanner.line, message, where);
}


void parserError(const Token& token, const std::string& message)
{
    if(token.type == TokenType::END_OF_FILE)
    {
        reportError(token.line, " at end of file!", message);
    }
    else
    {
        std::string s = " at end '";
        s += token.lexMe;
        s += "'";
        reportError(token.line, s, message);
    }
}