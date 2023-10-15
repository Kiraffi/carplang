#pragma once

#include <unordered_map>
#include <vector>

#include "expr.h"
#include "mytypes.h"
#include "scanner.h"
#include "statement.h"
#include "token.h"

struct MyMemory
{
    std::vector<std::string> strings;
    std::vector<Token> tokens;
    std::vector<Expr> expressions;
    std::vector<Statement> statements;
    std::unordered_map<std::string, ExprValue> variables;
    std::vector<u8> scriptFileData;
};
