#pragma once

#include "mytypes.h"

enum StatementType : u8
{
    StatementType_Expression,
    StatementType_Print,
    StatementType_VarDeclare,
    StatementType_Block,

    StatementType_Count,
};

struct Statement
{
    union
    {
        u32 expressionIndex;
        i32 blockIndex;
    };
    u32 tokenIndex;
    StatementType type;
};