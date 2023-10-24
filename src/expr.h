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
    LiteralType_Boolean,
    LiteralType_I64,
    LiteralType_Double,
    LiteralType_String,
    LiteralType_Identifier,
};

enum ExprType : u32
{
    ExprType_None,
    ExprType_Binary,
    ExprType_Grouping,
    ExprType_Literal,
    ExprType_Unary,
    ExprType_Variable,
    ExprType_Assign,
    ExprType_Logical,
    ExprType_CallFn,

};
struct ExprValue
{
    union
    {
        i64 value;
        double doubleValue;
        u32 stringIndex;
    };
    LiteralType literalType;
};
struct Expr
{
    union
    {
        ExprValue exprValue;
        u32 callParamAmount;
    };
    u32 tokenOperIndex;
    union
    {
        struct
        {
            //u32 secondTokenIndex;

            u32 leftExprIndex;
            u32 rightExprIndex;
        };
        struct
        {
            u32 callParams[4];
            u32 callee;
        };
    };
    //u32 myExprIndex;

    ExprType exprType;
};

