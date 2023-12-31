#include "astparser.h"

#include "errors.h"
#include "expr.h"
#include "helpers.h"
#include "interpreter.h"
#include "mymemory.h"


struct Parser
{
    MyMemory& mem;
    i32 currentPos;
};

static u32 expression(Parser& parser);
static u32 declaration(Parser& parser);
static i32 block(Parser& parser, i32 parentBlockIndex);


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

static u32 primary(Parser& parser)
{
    if(match(parser, TokenType::FALSE))
        return addExpr(parser.mem, { .exprValue = {.value = 0, .literalType = LiteralType_Boolean }, .exprType = ExprType_Literal,  });
    if(match(parser, TokenType::TRUE))
        return addExpr(parser.mem, { .exprValue = { .value = ~(i64(0)), .literalType = LiteralType_Boolean }, .exprType = ExprType_Literal,  });
    if(match(parser, TokenType::NIL))
        return addExpr(parser.mem, { .exprValue = { .value = 0, .literalType = LiteralType_Null}, .exprType = ExprType_Literal,  });
    if(match(parser, TokenType::IDENTIFIER))
    {
        const Token& prevToken = previous(parser);
        //const ExprValue& value = getConstValue(parser.mem, prevToken);
        return addExpr(parser.mem, { .exprValue = prevToken.value, .exprType = ExprType_Variable });
//                       { .exprValue = prevToken.value, .exprType = ExprType_Literal, });
    }
    if(match(parser, TokenType::STRING))
    {
        const Token& prevToken = previous(parser);
        return addExpr(parser.mem,
            { .exprValue = { .value = prevToken.value.value, .literalType = LiteralType_String }, .exprType = ExprType_Literal,  });
    }
    if(match(parser, TokenType::NUMBER))
    {
        const Token& prevToken = previous(parser);
        Expr newExpr = { .exprValue = { .value = prevToken.value.value, .literalType = LiteralType_Double }, .exprType = ExprType_Literal,};
        return addExpr(parser.mem, newExpr);
    }
    if(match(parser, TokenType::INTEGER))
    {
        const Token& prevToken = previous(parser);
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

static u32 finishCall(Parser& parser, u32 callee)
{
    Expr expr {
        .callee = callee,
        .exprType = ExprType::ExprType_CallFn
    };

    u32 args = 0;
    if(!check(parser, TokenType::RIGHT_PAREN))
    {
        do
        {
            if(args == 4)
            {
                reportError(parser.mem, peek(parser), "Only 4 arguments are legal on fn call!\n");
                DEBUG_BREAK_MACRO(-1);
            }

            u32 exprIndex = expression(parser);
            expr.callParams[args] = exprIndex;
            args++;
        } while(match(parser, TokenType::COMMA));

    }
    expr.callParamAmount = args;
    //why do we need? const Token& token =
    consume(parser, TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    u32 tokenIndex = parser.currentPos;
    expr.tokenOperIndex = tokenIndex;
    return addExpr(parser.mem, expr);

}

static u32 callFn(Parser& parser)
{
    u32 exprIndex = primary(parser);

    while(match(parser, TokenType::LEFT_PAREN))
    {
        exprIndex = finishCall(parser, exprIndex);
    }
    return exprIndex;
}


static u32 unary(Parser& parser)
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
    return callFn(parser);
}


static u32 factor(Parser& parser)
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




static u32 term(Parser& parser)
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

static u32 comparison(Parser& parser)
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



static u32 equality(Parser& parser)
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

static u32 logicalAnd(Parser& parser)
{
    u32 exprIndex = equality(parser);
    while(match(parser, TokenType::AND))
    {
        u32 prevIndex = previousIndex(parser);
        u32 rightIndex = equality(parser);

        Expr expr{
            .tokenOperIndex = prevIndex,
            .leftExprIndex = exprIndex,
            .rightExprIndex = rightIndex,
            .exprType = ExprType_Logical
        };

        exprIndex = addExpr(parser.mem, expr);

    }

    return exprIndex;
}

static u32 logicalOr(Parser& parser)
{
    u32 exprIndex = logicalAnd(parser);
    while(match(parser, TokenType::OR))
    {
        u32 prevIndex = previousIndex(parser);
        u32 rightIndex = equality(parser);

        Expr expr{
            .tokenOperIndex = prevIndex,
            .leftExprIndex = exprIndex,
            .rightExprIndex = rightIndex,
            .exprType = ExprType_Logical
        };

        exprIndex = addExpr(parser.mem, expr);

    }

    return exprIndex;
}

static u32 assignment(Parser& parser)
{
    u32 exprIndex = logicalOr(parser);

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
                .exprValue = expr.exprValue,
                //.tokenOperIndex = right.tokenOperIndex,
                .tokenOperIndex = prevIndex,
                //.tokenOperIndex = expr.exprValue.stringIndex,
                .rightExprIndex = exprRight,
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



static u32 declaration(Parser& parser)
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
        consume(parser, TokenType::SEMICOLON, "Expect ';' after variable declaration!");

        return addStatement(parser.mem, Statement{
            .expressionIndex = exprIndex,
            .tokenIndex = tokenIndex,
            .type = StatementType_VarDeclare
        });
    }
    else if(match(parser, TokenType::IF))
    {
        consume(parser, TokenType::LEFT_PAREN, "Expect '(' after if!");
        u32 exprIndex = expression(parser);
        consume(parser, TokenType::RIGHT_PAREN, "Expect ')' after if condition!");

        u32 statementIndex = declaration(parser);
        u32 elseStatementIndex = ~0u;
        if(match(parser, TokenType::ELSE))
        {
            elseStatementIndex = declaration(parser);
        }
        addStatement(parser.mem, Statement{
            .expressionIndex = exprIndex,
            .ifStatementIndex = statementIndex,
            .elseStatementIndex = elseStatementIndex,
            .type = StatementType_If
        });
    }
    else if(match(parser, TokenType::WHILE))
    {
        consume(parser, TokenType::LEFT_PAREN, "Expected '(' after while!");
        u32 conditionExprIndex = expression(parser);
        consume(parser, TokenType::RIGHT_PAREN, "Expected ')' after condition!");

        u32 whileStatementIndex = declaration(parser);

        return addStatement(parser.mem, Statement{
            .expressionIndex = conditionExprIndex,
            .whileStatementIndex = whileStatementIndex,
            .type = StatementType_While
        });
    }
    else if(match(parser, TokenType::PRINT))
    {
        u32 exprIndex = expression(parser);
        consume(parser, TokenType::SEMICOLON, "Expect ';' after expression!");
        return addStatement(parser.mem, Statement {
            .expressionIndex = exprIndex,
            .type = StatementType_Print
        });
    }
    else if(match(parser, TokenType::RETURN))
    {
        const Token& keyword = previous(parser);
        u32 exprIndex = ~0u;
        if(!check(parser, TokenType::SEMICOLON))
        {
            exprIndex = expression(parser);
        }
        consume(parser, TokenType::SEMICOLON, "Expect ';' after return value;");

        return addStatement(parser.mem, Statement{
            .expressionIndex = exprIndex,
            .type = StatementType_Return
        });
    }
    else if(match(parser, TokenType::FUNC))
    {
        const Token& name = consume(parser, TokenType::IDENTIFIER, "Expect function name");
        consume(parser, TokenType::LEFT_PAREN, "Expect '(' after function name");
        Statement stmnt{
            .type = StatementType_CallFn
        };
        u32 args = 0;
        if (!check(parser, TokenType::RIGHT_PAREN))
        {
            do
            {
                if (args >= 4)
                {
                    reportError(parser.mem, peek(parser), "Function can only have 4 parameters!");
                    DEBUG_BREAK_MACRO(10);
                }
                consume(parser, TokenType::IDENTIFIER, "Expected parameter name.");
                stmnt.paramsNameIndices[args] = previousIndex(parser);
                args++;
            } while (match(parser, TokenType::COMMA));
        }
        stmnt.paramsNameIndicesCount = args;
        consume(parser, TokenType::RIGHT_PAREN, "Expected ')' after parameters");
        consume(parser, TokenType::LEFT_BRACE, "Expected '{' before function body.");

        i32 blockIndex = block(parser, parser.mem.currentBlockIndex);
        Block& b = parser.mem.blocks[blockIndex];
        for(u32 i = 0; i < args; ++i)
        {
            const Token& t = parser.mem.tokens[stmnt.paramsNameIndices[i]];
            std::string& str = parser.mem.strings[t.value.stringIndex];
            b.variables.insert({str, ExprValue{}});
        }
        stmnt.blockIndex = blockIndex;
        u32 fnIndex = addStatement(parser.mem, stmnt);
        Block& b0 = parser.mem.blocks[0];
        b0.variables.insert({getConstString(parser.mem, name), ExprValue{.stringIndex = fnIndex }});
        return ~0;
    }
    else if(match(parser, TokenType::LEFT_BRACE))
    {
        i32 blockIndex = block(parser, 0);
        return addStatement(parser.mem, Statement {
            .blockIndex = blockIndex,
            .type = StatementType_Block
        });
    }

    else
    {
        u32 exprIndex = expression(parser);
        consume(parser, TokenType::SEMICOLON, "Expect ';' after expression!");
        return addStatement(parser.mem, Statement{
            .expressionIndex = exprIndex,
            .type = StatementType_Expression
        });
    }
}


static i32 block(Parser& parser, i32 parentBlockIndex)
{
    i32 blockIndex = parser.mem.blocks.size();
    parser.mem.blocks.emplace_back(Block{.parentBlockIndex = parentBlockIndex });
    //parser.mem.currentBlockIndex = blockIndex;
    Block& block = parser.mem.blocks[blockIndex];
    while(!check(parser, TokenType::RIGHT_BRACE) && !isAtEnd(parser))
    {
        block.statementIndices.push_back(declaration(parser));
    }
    consume(parser, TokenType::RIGHT_BRACE, "Expected '}' after block!");
    //parser.mem.currentBlockIndex = parentBlockIndex;
    return blockIndex;
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




static bool ast_test(MyMemory& mem)
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

    mem.blocks.emplace_back(Block{.parentBlockIndex = -1});
    while(!isAtEnd(parser))
    {
        u32 statementIndex = declaration(parser);
        if(statementIndex != ~0u)
            mem.blocks[0].statementIndices.push_back(statementIndex);
    }
    return true;
    //return ast_test(mem);
}