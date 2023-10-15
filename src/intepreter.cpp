#include "intepreter.h"

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
            exit(-2);
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
                exit(-4);
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
                        exit(-3);
                    }
                    value = -value;
                    break;
                case TokenType::BANG:
                    value = value == 0 ? ~(i64(0)) : 0;
                    break;
                default:
                    reportError(mem, token, "Not recognized unary type!");
                    exit(-4);
            }
            return ExprValue{ .value = value, .literalType = expr.exprValue.literalType };
        }
    }

    reportError(-2, "No known type!", "");
    exit(-3);
}




void intepret(MyMemory& mem, const Expr& expr)
{
    ExprValue value = evaluate(mem, expr);
    printf("%s\n", stringify(mem, value).data());

}