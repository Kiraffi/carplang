#pragma once

#include <unordered_map>
#include <vector>

#include "expr.h"
#include "mytypes.h"

struct Block
{
    i32 parentBlockIndex;
    std::vector<u32> statementIndices;
    std::unordered_map<std::string, ExprValue> variables;
};