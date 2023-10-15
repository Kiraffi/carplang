#include "astparser.h"

#include "errors.h"
#include "expr.h"
#include "mymemory.h"


struct Parser
{
    MyMemory& mem;
    i32 currentPos;
};

static u32 expression(Parser& parser);

static u32 addToken(MyMemory& mem, const Token& token)
{

    mem.scanner.tokens.emplace_back(token);
    return mem.scanner.tokens.size() - 1;
}

static u32 addExpr(MyMemory& mem, const Expr& expr)
{
    mem.expressions.emplace_back(expr);
    mem.expressions[mem.expressions.size() - 1].myExprIndex = mem.expressions.size() - 1;
    return mem.expressions.size() - 1;
}

static const Token LastToken{.type = TokenType::END_OF_FILE };
static const Token& peek(const Parser& parser)
{
    if (parser.currentPos >= parser.mem.scanner.tokens.size())
    {
        return LastToken;
    }
    return parser.mem.scanner.tokens[parser.currentPos];
}

static const Token& previous(const Parser& parser)
{
    i32 prevIndex = parser.currentPos - 1;
    prevIndex = prevIndex >= 0 ? prevIndex : 0;
    return parser.mem.scanner.tokens[prevIndex];
}

static const u32 previousIndex(const Parser& parser)
{
    i32 prevIndex = parser.currentPos - 1;
    prevIndex = prevIndex >= 0 ? prevIndex : 0;
    return prevIndex;
}

static const Token& advance(Parser& parser)
{
    if (!(peek(parser).type == TokenType::END_OF_FILE))
    {
        parser.currentPos++;
    }
    return previous(parser);
}

static bool isAtEnd(const Parser& parser)
{
    return peek(parser).type == TokenType::END_OF_FILE;
}


static bool check(const Parser& parser, TokenType type)
{
    if (isAtEnd(parser))
    {
        return false;
    }
    return peek(parser).type == type;
}


static bool match(Parser& parser, TokenType typeA)
{
    if(check(parser, typeA))
    {
        advance(parser);
        return true;
    }
    return false;
}

static bool match(Parser& parser, TokenType typeA, TokenType typeB)
{
    if(check(parser, typeA))
    {
        advance(parser);
        return true;
    }
    else
    {
        return match(parser, typeB);
    }
}

static bool match(Parser& parser, TokenType typeA, TokenType typeB, TokenType typeC)
{
    if(check(parser, typeA))
    {
        advance(parser);
        return true;
    }
    else
    {
        return match(parser, typeB, typeC);
    }
}

static bool match(Parser& parser, TokenType typeA, TokenType typeB, TokenType typeC, TokenType typeD)
{
    if(check(parser, typeA))
    {
        advance(parser);
        return true;
    }
    else
    {
        return match(parser, typeB, typeC, typeD);
    }
}


static bool parenthesize(const MyMemory& mem, const std::string& name, u32 leftIndex, u32 rightIndex,
    std::string& outStr)
{
    outStr.append("(");
    outStr.append(name);
    outStr.append(" ");
    bool result = printAst(mem, mem.expressions[leftIndex], outStr);
    outStr.append(" ");
    result &= printAst(mem, mem.expressions[rightIndex], outStr);
    outStr.append(")");
    return result;
}

static bool parenthesize(const MyMemory& mem, const std::string& name, u32 exprIndex,
    std::string& outStr)
{
    outStr.append("(");
    outStr.append(name);
    outStr.append(" ");
    bool result = printAst(mem, mem.expressions[exprIndex], outStr);
    outStr.append(")");
    return result;
}

static bool consume(Parser& parser, TokenType type, const std::string& message)
{
    if(check(parser, type))
    {
        advance(parser);
        return true;
    }

    parserError(peek(parser), message);
    // TODO FIX THIS!
    exit(-1);
    return false;
}

u32 primary(Parser& parser)
{
    if(match(parser, TokenType::FALSE))
        return addExpr(parser.mem, { .value = 0, .exprType = ExprType_Literal, .literalType = LiteralType_Boolean });
    if(match(parser, TokenType::TRUE))
        return addExpr(parser.mem, { .value = ~(i64(0)), .exprType = ExprType_Literal, .literalType = LiteralType_Boolean });
    if(match(parser, TokenType::NIL))
        return addExpr(parser.mem, { .value = 0, .exprType = ExprType_Literal, .literalType = LiteralType_Null });

    if(match(parser, TokenType::STRING))
    {
        const Expr& prevExpr = parser.mem.expressions[previousIndex(parser)];
        return addExpr(parser.mem,
            { .value = prevExpr.value, .exprType = ExprType_Literal, .literalType = LiteralType_String });
    }
    if(match(parser, TokenType::NUMBER))
    {
        const Expr& prevExpr = parser.mem.expressions[previousIndex(parser)];
        return addExpr(parser.mem,
                       { .value = prevExpr.value, .exprType = ExprType_Literal, .literalType = LiteralType_I64 });
    }
    if(match(parser, TokenType::LEFT_PAREN))
    {
        u32 newExpr = expression(parser);
        consume(parser, TokenType::RIGHT_PAREN, "Expect ')' after expression.");

    }
}


u32 unary(Parser& parser)
{
    u32 exprIndex = unary(parser);

    if(match(parser, TokenType::BANG, TokenType::MINUS))
    {
        u32 prevIndex = previousIndex(parser);
        u32 rightIndex = unary(parser);

        Expr expr{
            .tokenOperIndex = prevIndex,
            .rightExprIndex = rightIndex,
            .exprType = ExprType_Unary
        };

        exprIndex = addExpr(parser.mem, expr);
        return exprIndex;
    }
    return primary(parser);
}


u32 factor(Parser& parser)
{
    u32 exprIndex = unary(parser);

    while(match(parser, TokenType::SLASH, TokenType::STAR))
    {
        u32 prevIndex = previousIndex(parser);
        u32 rightIndex = unary(parser);

        Expr expr{
            .tokenOperIndex = prevIndex,
            .leftExprIndex = exprIndex,
            .rightExprIndex = rightIndex,
            .exprType = ExprType_Binary
        };

        exprIndex = addExpr(parser.mem, expr);

    }
    return exprIndex;
}




u32 term(Parser& parser)
{
    u32 exprIndex = factor(parser);

    while(match(parser, TokenType::MINUS, TokenType::PLUS))
    {
        u32 prevIndex = previousIndex(parser);
        u32 rightIndex = factor(parser);

        Expr expr{
            .tokenOperIndex = prevIndex,
            .leftExprIndex = exprIndex,
            .rightExprIndex = rightIndex,
            .exprType = ExprType_Binary
        };

        exprIndex = addExpr(parser.mem, expr);

    }
    return exprIndex;
}

u32 comparison(Parser& parser)
{
    u32 exprIndex = term(parser);

    while(match(parser, TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESSER, TokenType::LESSER_EQUAL))
    {
        u32 prevIndex = previousIndex(parser);
        u32 rightIndex = term(parser);

        Expr expr{
            .tokenOperIndex = prevIndex,
            .leftExprIndex = exprIndex,
            .rightExprIndex = rightIndex,
            .exprType = ExprType_Binary
        };

        exprIndex = addExpr(parser.mem, expr);
    }
    return exprIndex;
}



u32 equality(Parser& parser)
{
    u32 exprIndex = comparison(parser);

    while(match(parser, TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL))
    {
        u32 prevIndex = previousIndex(parser);
        u32 rightIndex = comparison(parser);

        Expr expr{
            .tokenOperIndex = prevIndex,
            .leftExprIndex = exprIndex,
            .rightExprIndex = rightIndex,
            .exprType = ExprType_Binary
        };

        exprIndex = addExpr(parser.mem, expr);
    }
    return exprIndex;
}





static u32 expression(Parser& parser)
{
    return equality(parser);
}




bool printAst(const MyMemory& mem, const Expr& expr, std::string& printStr)
{
    switch (expr.exprType)
    {
        case ExprType_None:
        {
            printf("ExprType is none\n!");
            return false;
        }
        case ExprType_Binary:
        {
            const std::string& lexMe = mem.scanner.tokens[expr.tokenOperIndex].lexMe;
            if(!parenthesize(mem, lexMe, expr.leftExprIndex, expr.rightExprIndex, printStr))
            {
                return false;
            }
        }
        break;
        case ExprType_Grouping:
        {
            if(!parenthesize(mem, "group", expr.rightExprIndex, printStr))
            {
                return false;
            }
        }
        break;
        case ExprType_Literal:
        {
            switch (expr.literalType)
            {
            case LiteralType_None: printf("Literaltype none!\n"); break;
            case LiteralType_Null: printStr.append("nil"); break;
            case LiteralType_I64: printStr.append(std::to_string(expr.value)); break;
            case LiteralType_Double: printStr.append(std::to_string(expr.doubleValue)); break;
            case LiteralType_String: printStr.append(mem.strings[expr.stringIndex]); break;
            }
        }
        break;
        case ExprType_Unary:
        {
            const std::string& lexMe = mem.scanner.tokens[expr.tokenOperIndex].lexMe;
            if (!parenthesize(mem, lexMe, expr.rightExprIndex, printStr))
            {
                return false;
            }
        }
        break;
    }

    return true;
}




bool ast_test(MyMemory& mem)
{
    u32 minusTokenIndex = addToken(mem, Token{ .lexMe = "-", .line = 1, .type = TokenType::MINUS });
    u32 starTokenIndex = addToken(mem, Token{ .lexMe = "*", .line = 1, .type = TokenType::STAR });

    Expr u64Lit{ .value = 123, .exprType = ExprType_Literal, .literalType = LiteralType_I64 };
    u32 u64ExpressionIndex = addExpr(mem, u64Lit);

    Expr doubleLit{ .doubleValue = 45.67, .exprType = ExprType_Literal, .literalType = LiteralType_Double };
    u32 doubleExpressionIndex= addExpr(mem, doubleLit);

    Expr unaryExpr{ .tokenOperIndex = minusTokenIndex, .rightExprIndex = u64ExpressionIndex, .exprType = ExprType_Unary };
    u32 unaryExpressionIndex = addExpr(mem, unaryExpr);

    Expr grouping{ .rightExprIndex = doubleExpressionIndex, .exprType = ExprType_Grouping };
    u32 groupingExpressionIndex = addExpr(mem, grouping);

    Expr expr{
        .tokenOperIndex = starTokenIndex,
        .leftExprIndex = unaryExpressionIndex,
        .rightExprIndex = groupingExpressionIndex,
        .exprType = ExprType_Binary
    };
    std::string s;

    printAst(mem, u64Lit, s);
    printf("%s\n", s.data());
    s.clear();
    printAst(mem, doubleLit, s);
    printf("%s\n", s.data());
    s.clear();
    printAst(mem, unaryExpr, s);
    printf("%s\n", s.data());
    s.clear();
    printAst(mem, grouping, s);
    printf("%s\n", s.data());
    s.clear();
    printAst(mem, expr, s);
    printf("%s\n", s.data());

    return true;
}

bool ast_generate(MyMemory& mem)
{
    return ast_test(mem);
}