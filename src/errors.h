#pragma once

#include <string.h>
#include <string>


#define LOG_ERROR(str) printf("%s in file: %s, line: %i\n", str, __FILE__, __LINE__)

struct Scanner;

void reportError(Scanner& scanner, const std::string& message, const std::string& where);

