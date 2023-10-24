#include "helpers.h"

#include "errors.h"
#include "expr.h"
#include "token.h"

#include <algorithm>
#include <assert.h>
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
    //mem.expressions[mem.expressions.size() - 1].myExprIndex = mem.expressions.size() - 1;
    return mem.expressions.size() - 1;
}

u32 addString(MyMemory& mem, const std::string& str)
{
    mem.strings.emplace_back(str);
    return mem.strings.size() - 1;
}

u32 addStatement(MyMemory& mem, const Statement& statement)
{
    if(statement.type != StatementType_CallFn)
    {
        mem.statements.emplace_back(statement);
        return mem.statements.size() - 1;
    }
    mem.functions.emplace_back(statement);
    return mem.functions.size() - 1;

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
            return getConstString(mem, exprValue);
    }

    reportError(-1, "Literal type unknown", "");
    DEBUG_BREAK_MACRO(-5);
}

const ExprValue& getConstValue(const MyMemory& mem, const std::string& findName, u32 blockIndex)
{
    const Block& block = mem.blocks[blockIndex];
    auto iter = block.variables.find(findName);
    if(iter == block.variables.end())
    {
        if(block.parentBlockIndex >= 0 && block.parentBlockIndex < mem.blocks.size())
        {
            return getConstValue(mem, findName, block.parentBlockIndex);
        }

        reportError(mem, Token{}, "Variable not found!");
        DEBUG_BREAK_MACRO(20);
    }
    return iter->second;

}
const ExprValue& getConstValue(const MyMemory& mem, u32 stringIndex)
{
    const std::string &findName = mem.strings[stringIndex];
    return getConstValue(mem, findName, mem.currentBlockIndex);
}
const ExprValue& getConstValue(const MyMemory& mem, const ExprValue& exprValue)
{
    return getConstValue(mem, exprValue.stringIndex);
}
const ExprValue& getConstValue(const MyMemory& mem, const Token& token)
{
    return getConstValue(mem, token.value);
}

ExprValue& getMutableValue(MyMemory& mem, const std::string& findName, u32 blockIndex)
{
    Block& block = mem.blocks[blockIndex];
    auto iter = block.variables.find(findName);
    if(iter == block.variables.end())
    {
        if(block.parentBlockIndex >= 0 && block.parentBlockIndex < mem.blocks.size())
        {
            return getMutableValue(mem, findName, block.parentBlockIndex);
        }

        reportError(mem, Token{}, "Variable not found!");
        DEBUG_BREAK_MACRO(20);
    }
    return iter->second;

}

ExprValue& getMutableValue(MyMemory& mem, u32 stringIndex)
{
    const std::string& findName = mem.strings[stringIndex];
    return getMutableValue(mem, findName, mem.currentBlockIndex);
}
ExprValue& getMutableValue(MyMemory& mem, const ExprValue& exprValue)
{
    return getMutableValue(mem, exprValue.stringIndex);
}
ExprValue& getMutableValue(MyMemory& mem, const Token& token)
{
    return getMutableValue(mem, token.value);
}

void defineVariable(MyMemory& mem, const std::string& name, const ExprValue& value)
{
    Block& b = mem.blocks[mem.currentBlockIndex];
    auto iter = b.variables.find(name);
    if(iter != b.variables.end())
    {
        reportError(-12, "Variable already exists!", "");
        DEBUG_BREAK_MACRO(20);
    }
    b.variables.insert({name, value});
}

std::string& getMutableString(MyMemory& mem, const Token& token)
{
    assert(token.type == TokenType::IDENTIFIER);
    assert(token.value.stringIndex < mem.strings.size());
    return mem.strings[token.value.stringIndex];
}

std::string& getMutableString(MyMemory& mem, const ExprValue& exprValue)
{
    assert(exprValue.literalType == LiteralType_Identifier || exprValue.literalType == LiteralType_String);
    assert(exprValue.stringIndex < mem.strings.size());
    return mem.strings[exprValue.stringIndex];
}

const std::string& getConstString(const MyMemory& mem, const Token& token)
{
    assert(token.type == TokenType::IDENTIFIER);
    assert(token.value.stringIndex < mem.strings.size());
    return mem.strings[token.value.stringIndex];
}

const std::string& getConstString(const MyMemory& mem, const ExprValue& exprValue)
{
    assert(exprValue.literalType == LiteralType_Identifier || exprValue.literalType == LiteralType_String);
    assert(exprValue.stringIndex < mem.strings.size());
    return mem.strings[exprValue.stringIndex];
}
