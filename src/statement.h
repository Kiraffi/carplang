#pragma once

#include "mytypes.h"

enum StatementType : u8
{
    StatementType_Expression,
    StatementType_Print,

    StatementType_Count,
};

struct Statement
{
    u32 expressionIndex;
    StatementType type;
};