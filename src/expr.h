#pragma once

#include "mytypes.h"

#include <string>

struct Token;

enum ExprValues : u32
{
    ExprValues_None = 0,
    ExprValues_Has_Left = 1,
    ExprValues_Has_Right = ExprValues_Has_Left << 1,
    ExprValues_Has_Token = ExprValues_Has_Right << 1,

    ExprValues_Has_Literal_String = ExprValues_Has_Token << 1,
    ExprValues_Has_Literal_Double = ExprValues_Has_Literal_String << 1,
    ExprValues_Has_Literal_U64 = ExprValues_Has_Literal_Double << 1,

};

enum LiteralType : u32
{
    LiteralType_None,
    LiteralType_Null,
    LiteralType_U64,
    LiteralType_Double,
    LiteralType_String,
};

enum ExprType : u32
{
    ExprType_None,
    ExprType_Binary,
    ExprType_Grouping,
    ExprType_Literal,
    ExprType_Unary,
};

struct Expr
{
    union
    {
        i64 value;
        double doubleValue;
        char* valueStr;
    };

    u32 tokenOperIndex;
    u32 secondTokenIndex;

    u32 leftExprIndex;
    u32 rightExprIndex;

    ExprType exprType;
    LiteralType literalType;
};


