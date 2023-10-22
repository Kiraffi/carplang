#include "astparser.h"

#include "errors.h"
#include "expr.h"
#include "helpers.h"
#include "intepreter.h"
#include "mymemory.h"


struct Parser
{
    MyMemory& mem;
    i32 currentPos;
};

static u32 expression(Parser& parser);


static const Token LastToken{.type = TokenType::END_OF_FILE };
static const Token& peek(const Parser& parser)
{
    if (parser.currentPos >= parser.mem.tokens.size())
    {
        return LastToken;
    }
    return parser.mem.tokens[parser.currentPos];
}

static const Token& previous(const Parser& parser)
{
    i32 prevIndex = parser.currentPos - 1;
    prevIndex = prevIndex >= 0 ? prevIndex : 0;
    return parser.mem.tokens[prevIndex];
}

static const u32 previousIndex(const Parser& parser)
{
    i32 prevIndex = parser.currentPos - 1;
    prevIndex = prevIndex >= 0 ? prevIndex : 0;
    return prevIndex;
}

static bool isAtEnd(const Parser& parser)
{
    return peek(parser).type == TokenType::END_OF_FILE;
}

static const Token& advance(Parser& parser)
{
    if (!isAtEnd(parser))
    {
        parser.currentPos++;
    }
    return previous(parser);
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

static const Token& consume(Parser& parser, TokenType type, const std::string& message)
{
    if(check(parser, type))
    {
        return advance(parser);
    }

    reportError(parser.mem, peek(parser), message);
    // TODO FIX THIS!
    DEBUG_BREAK_MACRO(-1);
}

u32 primary(Parser& parser)
{
    if(match(parser, TokenType::FALSE))
        return addExpr(parser.mem, { .exprValue = {.value = 0, .literalType = LiteralType_Boolean }, .exprType = ExprType_Literal,  });
    if(match(parser, TokenType::TRUE))
        return addExpr(parser.mem, { .exprValue = { .value = ~(i64(0)), .literalType = LiteralType_Boolean }, .exprType = ExprType_Literal,  });
    if(match(parser, TokenType::NIL))
        return addExpr(parser.mem, { .exprValue = { .value = 0, .literalType = LiteralType_Null}, .exprType = ExprType_Literal,  });
    if(match(parser, TokenType::IDENTIFIER))
    {
        const Token& prevToken = parser.mem.tokens[previousIndex(parser)];
        //const ExprValue& value = getConstValue(parser.mem, prevToken);
        return addExpr(parser.mem, { .exprValue = prevToken.value, .tokenOperIndex = prevToken.value.stringIndex, .exprType = ExprType_Variable });
//                       { .exprValue = prevToken.value, .exprType = ExprType_Literal, });
    }
    if(match(parser, TokenType::STRING))
    {
        const Token& prevToken = parser.mem.tokens[previousIndex(parser)];
        return addExpr(parser.mem,
            { .exprValue = { .value = prevToken.value.value, .literalType = LiteralType_String }, .exprType = ExprType_Literal,  });
    }
    if(match(parser, TokenType::NUMBER))
    {
        const Token& prevToken = parser.mem.tokens[previousIndex(parser)];
        Expr newExpr = { .exprValue = { .value = prevToken.value.value, .literalType = LiteralType_Double }, .exprType = ExprType_Literal,};
        return addExpr(parser.mem, newExpr);
    }
    if(match(parser, TokenType::INTEGER))
    {
        const Token& prevToken = parser.mem.tokens[previousIndex(parser)];
        Expr newExpr = { .exprValue = { .value = prevToken.value.value, .literalType = LiteralType_I64 }, .exprType = ExprType_Literal,};
        return addExpr(parser.mem, newExpr);
    }
    if(match(parser, TokenType::LEFT_PAREN))
    {
        u32 newExpr = expression(parser);
        consume(parser, TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return addExpr(parser.mem, parser.mem.expressions[newExpr]);
    }

    reportError(parser.mem, peek(parser), "No matching type for primary!\n");
    DEBUG_BREAK_MACRO(-1);
}


u32 unary(Parser& parser)
{
    if(match(parser, TokenType::BANG, TokenType::MINUS))
    {
        u32 prevIndex = previousIndex(parser);
        u32 rightIndex = unary(parser);

        Expr expr{
            .tokenOperIndex = prevIndex,
            .rightExprIndex = rightIndex,
            .exprType = ExprType_Unary
        };

        u32 exprIndex = addExpr(parser.mem, expr);
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


static u32 assignment(Parser& parser)
{
    u32 exprIndex = equality(parser);

    while(match(parser, TokenType::EQUAL))
    {
        u32 prevIndex = previousIndex(parser);
        u32 exprRight = assignment(parser);

        const Expr& right = parser.mem.expressions[exprRight];
        const Expr& expr = parser.mem.expressions[exprIndex];
        if(expr.exprType == ExprType_Variable)
        {
            const Token& token = parser.mem.tokens[right.tokenOperIndex];
            const std::string& name = parser.mem.strings[expr.exprValue.stringIndex];
            Expr newExpr{
                .exprValue = right.exprValue,
                .tokenOperIndex = expr.exprValue.stringIndex,
                .exprType = ExprType_Assign
            };
            return addExpr(parser.mem, newExpr);
        }
        reportError(parser.mem, parser.mem.tokens[prevIndex], "Invalid target assignment!\n");

        LOG_ERROR("Invalid target assignment!");
        DEBUG_BREAK_MACRO(-30);
    }
    return exprIndex;
}



static u32 expression(Parser& parser)
{
    u32 exprIndex = assignment(parser);

    return exprIndex;
}



static Statement statement(Parser& parser)
{
    if(match(parser, TokenType::VAR))
    {
        u32 tokenIndex = parser.currentPos;
        consume(parser, TokenType::IDENTIFIER, "Expected variable name!");
        if(!match(parser, TokenType::EQUAL))
        {
            reportError(parser.mem, peek(parser), "variable not set!");
            DEBUG_BREAK_MACRO(10);
        }
        u32 exprIndex = expression(parser);
        Expr& expr = parser.mem.expressions[exprIndex];
        expr.tokenOperIndex = tokenIndex;
        consume(parser, TokenType::SEMICOLON, "Expect ';' after variable declaration!");

        // wrong place!
        //defineVariable(parser.mem, parser.mem.strings[parser.mem.tokens[tokenIndex].value.stringIndex], expr.exprValue);

        return Statement{ .expressionIndex = exprIndex, .tokenIndex = tokenIndex, .type = StatementType_VarDeclare };
    }
    else if(match(parser, TokenType::PRINT))
    {
        u32 exprIndex = expression(parser);
        consume(parser, TokenType::SEMICOLON, "Expect ';' after expression!");
        return Statement{ .expressionIndex = exprIndex, .type = StatementType_Print };
    }
    else
    {
        u32 exprIndex = expression(parser);
        consume(parser, TokenType::SEMICOLON, "Expect ';' after expression!");
        return Statement{ .expressionIndex = exprIndex, .type = StatementType_Expression };
    }
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
            const std::string& lexMe = getTokenValueAsString(mem, mem.tokens[expr.tokenOperIndex]);
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
            switch (expr.exprValue.literalType)
            {
            case LiteralType_None: printf("Literaltype none!\n"); break;
            case LiteralType_Null: printStr.append("nil"); break;
            case LiteralType_I64: printStr.append(std::to_string(expr.exprValue.value)); break;
            case LiteralType_Double: printStr.append(std::to_string(expr.exprValue.doubleValue)); break;
            case LiteralType_String: printStr.append(mem.strings[expr.exprValue.stringIndex]); break;
            case LiteralType_Boolean: printStr.append("boolean"); break;
            }
        }
        break;
        case ExprType_Unary:
        {
            const std::string& lexMe = getTokenValueAsString(mem, mem.tokens[expr.tokenOperIndex]);
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
    u32 minusStr = addString(mem, "-");
    u32 starStr = addString(mem, "*");
    u32 minusTokenIndex = addToken(mem, Token{ .value{.stringIndex = minusStr, .literalType = LiteralType_String }, .line = 1, .type = TokenType::MINUS });
    u32 starTokenIndex = addToken(mem, Token{ .value{.stringIndex = starStr, .literalType = LiteralType_String }, .line = 1, .type = TokenType::STAR });

    Expr u64Lit{ .exprValue = { .value = 123, .literalType = LiteralType_I64 }, .exprType = ExprType_Literal,  };
    u32 u64ExpressionIndex = addExpr(mem, u64Lit);

    Expr doubleLit{ .exprValue = { .doubleValue = 45.67, .literalType = LiteralType_Double }, .exprType = ExprType_Literal,  };
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
    Parser parser {.mem = mem, .currentPos = 0 };
    while(!isAtEnd(parser))
    {
        addStatement(mem, statement(parser));
    }
    return true;
    //return ast_test(mem);
}