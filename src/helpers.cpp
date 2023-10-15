#include "helpers.h"

#include "errors.h"
#include "expr.h"
#include "token.h"

#include <algorithm>
#include <unordered_map>
#include <vector>

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

u32 addStatement(MyMemory& mem, const Statement& statement)
{
    mem.statements.emplace_back(statement);
    return mem.statements.size() - 1;
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
            return exprValue.value == 0 ? "false" : "true";
        case LiteralType_I64:
            return std::to_string(exprValue.value);
        case LiteralType_Double:
            return std::to_string(exprValue.doubleValue);
        case LiteralType_String:
            return mem.strings[exprValue.stringIndex];
    }

    reportError(-1, "Literal type unknown", "");
    exit(-5);
}

ExprValue& getMutableValue(MyMemory& mem, const Token& token)
{
    const std::string& findName = mem.strings[token.value.stringIndex];
    auto iter = mem.variables.find(findName);
    if(iter == mem.variables.end())
    {
        reportError(mem, token, "Variable not found!");
        exit(20);
    }
    return iter->second;
}

const ExprValue& getConstValue(const MyMemory& mem, const Token& token)
{
    const std::string& findName = mem.strings[token.value.stringIndex];

    auto iter = mem.variables.find(findName);
    if(iter == mem.variables.end())
    {
        reportError(mem, token, "Variable not found!");
        exit(20);
    }
    return iter->second;
}

void defineVariable(MyMemory& mem, const std::string& name, const ExprValue& value)
{
    auto iter = mem.variables.find(name);
    if(iter != mem.variables.end())
    {
        reportError(-12, "Variable already exists!", "");
        exit(20);
    }
    mem.variables.insert({name, value});
}

