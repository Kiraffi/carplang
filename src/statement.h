#pragma once

#include "mytypes.h"

enum StatementType : u8
{
    StatementType_Expression,
    StatementType_Print,
    StatementType_VarDeclare,
    StatementType_Block,

    StatementType_If,

    StatementType_Count,
};

struct Statement
{
    union
    {
        u32 expressionIndex;
        i32 blockIndex;
    };
    union
    {
        u32 tokenIndex;
        struct
        {
            u32 ifStatementIndex;
            u32 elseStatementIndex;
        };
    };
    StatementType type;
};