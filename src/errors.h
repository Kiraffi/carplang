#pragma once

#include <string.h>
#include <string>

#include "mytypes.h"

struct Scanner;
struct Token;

#define LOG_ERROR(str) printf("%s in file: %s, line: %i\n", str, __FILE__, __LINE__)

struct Scanner;
void reportError(i32 line, const std::string& message, const std::string& where);
void reportError(Scanner& scanner, const std::string& message, const std::string& where);
void parserError(const Token& token, const std::string& message);

