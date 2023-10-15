#pragma once

#include "mytypes.h"

enum StatementType : u8
{
    StatementType_Expression,
    StatementType_Print,
    StatementType_VarDeclare,

    StatementType_Count,
};

struct Statement
{
    u32 expressionIndex;
    u32 tokenIndex;
    StatementType type;
};