#include "errors.h"
#include "tokens.h"


void reportError(Scanner& scanner, const std::string& message, const std::string& where)
{
    scanner.hasErrors = true;
    printf("[line: %i] Error %s: %s\n", scanner.line, where.data(), message.data());
}