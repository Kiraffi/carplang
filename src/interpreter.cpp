#include "interpreter.h"

#include "errors.h"
#include "expr.h"
#include "helpers.h"
#include "mymemory.h"
#include "token.h"

#include <cmath>
#include <string>

static constexpr i64 NegFull = ~i64(0);

static ExprValue doDoubleOperOnBinary(TokenType type, double a, double b)
{
    ExprValue value{.literalType = LiteralType_Double };

    switch(type)
    {
        case TokenType::MINUS:
            value.doubleValue = a - b;
            break;
        case TokenType::PLUS:
            value.doubleValue = a + b;
            break;
        case TokenType::STAR:
            value.doubleValue = a * b;
            break;
        case TokenType::SLASH:
            value.doubleValue = a / b;
            break;

        case TokenType::GREATER:
            value.value = a > b ? NegFull : 0;
            value.literalType = LiteralType_Boolean;
            break;
        case TokenType::GREATER_EQUAL:
            value.value = a >= b ? NegFull : 0;
            value.literalType = LiteralType_Boolean;
            break;
        case TokenType::LESSER:
            value.value = a < b ? NegFull : 0;
            value.literalType = LiteralType_Boolean;
            break;
        case TokenType::LESSER_EQUAL:
            value.value = a <= b ? NegFull : 0;
            value.literalType = LiteralType_Boolean;
            break;
        case TokenType::BANG_EQUAL:
            value.value = a != b ? NegFull : 0;
            value.literalType = LiteralType_Boolean;
            break;
        case TokenType::EQUAL_EQUAL:
            value.value = a == b ? NegFull : 0;
            value.literalType = LiteralType_Boolean;
            break;
        default:
            value.value = NegFull;
            value.literalType = LiteralType_None;
    }
    return value;
}

static ExprValue doIntOperOnBinary(TokenType type, i64 a, i64 b)
{
    ExprValue value{.literalType = LiteralType_I64 };

    switch(type)
    {
        case TokenType::MINUS:
            value.value = a - b;
            break;
        case TokenType::PLUS:
            value.value = a + b;
            break;
        case TokenType::STAR:
            value.value = a * b;
            break;
        case TokenType::SLASH:
            value.value = a / b;
            break;

        case TokenType::GREATER:
            value.value = a > b ? NegFull : 0;
            value.literalType = LiteralType_Boolean;
            break;
        case TokenType::GREATER_EQUAL:
            value.value = a >= b ? NegFull : 0;
            value.literalType = LiteralType_Boolean;
            break;
        case TokenType::LESSER:
            value.value = a < b ? NegFull : 0;
            value.literalType = LiteralType_Boolean;
            break;
        case TokenType::LESSER_EQUAL:
            value.value = a <= b ? NegFull : 0;
            value.literalType = LiteralType_Boolean;
            break;
        case TokenType::BANG_EQUAL:
            value.value = a != b ? NegFull : 0;
            value.literalType = LiteralType_Boolean;
            break;
        case TokenType::EQUAL_EQUAL:
            value.value = a == b ? NegFull : 0;
            value.literalType = LiteralType_Boolean;
            break;
        default:
            value.value = NegFull;
            value.literalType = LiteralType_None;
    }
    return value;
}


static ExprValue evaluate(MyMemory& mem, const Expr& expr)
{
    switch(expr.exprType)
    {
        case ExprType_None:
        {
            reportError(-1, "Expr type none!", "");
            DEBUG_BREAK_MACRO(-2);
        }
        break;
        case ExprType_Binary:
        {
            const ExprValue& leftValue = evaluate(mem, mem.expressions[expr.leftExprIndex]);
            const ExprValue& rightValue = evaluate(mem, mem.expressions[expr.rightExprIndex]);
            const Token& token = getTokenOper(mem, expr);

            if(checkNumber(leftValue) && checkNumber(rightValue))
            {
                if(leftValue.literalType == LiteralType_Double || rightValue.literalType == LiteralType_Double)
                    return doDoubleOperOnBinary(token.type, getDouble(leftValue), getDouble(rightValue));
                return doIntOperOnBinary(token.type, getInt(leftValue), getInt(rightValue) );
            }
            else if(checkString(leftValue) && checkString(rightValue))
            {
                ExprValue newValue {.literalType = LiteralType_String };
                std::string s = mem.strings[leftValue.stringIndex];
                s += mem.strings[rightValue.stringIndex];
                newValue.stringIndex = addString(mem, s);

                return newValue;

            }
            else
            {
                reportError(mem, token, "Left and Right values aren't matching");
                DEBUG_BREAK_MACRO(-4);
            }
        }
        break;
        case ExprType_Grouping:
        {

        }
        break;
        case ExprType_Literal:
        {
            return expr.exprValue;
        }
        case ExprType_Unary:
        {
            const ExprValue& exprValue = getRightExpr(mem, expr).exprValue;
            const Token& token = getTokenOper(mem, expr);
            i64 value = exprValue.value;
            switch(token.type)
            {
                case TokenType::MINUS:
                    if(!checkNumber(exprValue))
                    {
                        reportError(mem, token, "Unary not number");
                        DEBUG_BREAK_MACRO(-3);
                    }
                    value = -value;
                    break;
                case TokenType::BANG:
                    value = value == 0 ? ~(i64(0)) : 0;
                    break;
                default:
                    reportError(mem, token, "Not recognized unary type!");
                    DEBUG_BREAK_MACRO(-4);
            }
            return ExprValue{ .value = value, .literalType = expr.exprValue.literalType };
        }
        case ExprType_Variable:
        {
            return getConstValue(mem, expr.exprValue);
        }

        case ExprType_Assign:
        {
            const ExprValue& rightValue = evaluate(mem, mem.expressions[expr.rightExprIndex]);

            ExprValue& mutableValue = getMutableValue(mem, expr.exprValue);
            mutableValue = rightValue;
            return mutableValue;
        }
    }

    reportError(-2, "No known type!", "");
    DEBUG_BREAK_MACRO(-3);
}


void interpret(MyMemory& mem, const Statement& statement)
{
    std::string s;
    switch(statement.type)
    {
        case StatementType_Expression:
        {
            const Expr& expr = mem.expressions[statement.expressionIndex];
            evaluate(mem, expr);
        }
        break;
        case StatementType_Block:
        {
            const Block& b = mem.blocks[statement.blockIndex];
            mem.currentBlockIndex = statement.blockIndex;
            for(i32 index : b.statementIndices)
            {
                interpret(mem, mem.statements[index]);
            }
            mem.currentBlockIndex = b.parentBlockIndex;
        }
            break;
        case StatementType_Print:
        {
            const Expr& expr = mem.expressions[statement.expressionIndex];
            printf("%s\n", stringify(mem, evaluate(mem, expr)).data());
        }
        break;
        case StatementType_VarDeclare:
        {
            const Expr& expr = mem.expressions[statement.expressionIndex];
            ExprValue value = evaluate(mem, expr);
            defineVariable(mem, mem.strings[mem.tokens[statement.tokenIndex].value.stringIndex], value);
        }
        break;
        case StatementType_Count:
        {
            reportError(-3, "Statement count", "");
            DEBUG_BREAK_MACRO(-50);
        }
        break;

    }
}

void interpret(MyMemory& mem, const Expr& expr)
{
    ExprValue value = evaluate(mem, expr);
    printf("%s\n", stringify(mem, value).data());

}