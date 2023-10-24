#pragma once

#include <unordered_map>
#include <vector>

#include "block.h"
#include "expr.h"
#include "mytypes.h"
#include "scanner.h"
#include "statement.h"
#include "token.h"

struct MyMemory
{
    i32 currentBlockIndex;
    i32 statementIndex;
    std::vector<Block> blocks;
    std::vector<std::string> strings;
    std::vector<Token> tokens;
    std::vector<Expr> expressions;
    std::vector<Statement> statements;
    std::vector<u8> scriptFileData;
    std::vector<Statement> functions;
};
