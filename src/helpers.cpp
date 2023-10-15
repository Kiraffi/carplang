#include "helpers.h"

#include "errors.h"

u32 addToken(MyMemory& mem, const Token& token)
{
    mem.tokens.emplace_back(token);
    return mem.tokens.size() - 1;
}

u32 addExpr(MyMemory& mem, const Expr& expr)
{
    mem.expressions.emplace_back(expr);
    mem.expressions[mem.expressions.size() - 1].myExprIndex = mem.expressions.size() - 1;
    return mem.expressions.size() - 1;
}

u32 addString(MyMemory& mem, const std::string& str)
{
    mem.strings.emplace_back(str);
    return mem.strings.size() - 1;
}

const Token& getTokenOper(const MyMemory& mem, const Expr& expr)
{
    return mem.tokens[expr.tokenOperIndex];
}

const Expr& getLeftExprValue(const MyMemory& mem, const Expr& expr)
{
    const Expr& left = mem.expressions[expr.leftExprIndex];
    return left;
}

const Expr& getRightExpr(const MyMemory& mem, const Expr& expr)
{
    const Expr& right = mem.expressions[expr.rightExprIndex];
    return right;
}

bool checkNumber(const ExprValue& exprValue)
{
    return exprValue.literalType == LiteralType_Double || exprValue.literalType == LiteralType_I64;
}
bool checkString(const ExprValue& exprValue)
{
    return exprValue.literalType == LiteralType_String;
}

double getDouble(const ExprValue& exprValue)
{
    return exprValue.literalType == LiteralType_Double ? exprValue.doubleValue : (double)exprValue.value;
}

i64 getInt(const ExprValue& exprValue)
{
    return exprValue.literalType == LiteralType_I64 ? exprValue.value : (i64)exprValue.doubleValue;
}

std::string stringify(const MyMemory& mem, const ExprValue& exprValue)
{
    switch(exprValue.literalType)
    {
        case LiteralType_None:
            return "NONE!!!!!!";
        case LiteralType_Null:
            return "nil";
        case LiteralType_Boolean:
            return exprValue.value == 0 ? "False" : "True";
        case LiteralType_I64:
            return std::to_string(exprValue.value);
        case LiteralType_Double:
            return std::to_string(exprValue.doubleValue);
        case LiteralType_String:
            return mem.strings[exprValue.stringIndex];
    }

    reportError(-1, "Unexpected return", "");
    exit(-5);
}


