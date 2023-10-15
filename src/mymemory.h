#pragma once

#include <vector>

#include "expr.h"
#include "mytypes.h"
#include "scanner.h"
#include "token.h"

struct MyMemory
{
    std::vector<std::string> strings;
    std::vector<Token> tokens;
    std::vector<Expr> expressions;
    std::vector<u8> scriptFileData;
};
