#include "astparser.h"

#include "expr.h"
#include "mymemory.h"


struct Parser
{
    MyMemory& mem;
    i32 currentPos;
};


static const Token& peek(const Parser& parser)
{
    if (parser.currentPos >= parser.mem.scanner.tokens.size())
    {
        return Token{.type = TokenType::END_OF_FILE };
    }
    return parser.mem.scanner.tokens[parser.currentPos];
}

static const Token& previous(const Parser& parser)
{
    i32 prevIndex = parser.currentPos - 1;
    prevIndex = prevIndex >= 0 ? prevIndex : 0;
    return parser.mem.scanner.tokens[prevIndex];
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
            case LiteralType_U64: printStr.append(std::to_string(expr.value)); break;
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
    mem.scanner.tokens.emplace_back(Token{ .lexMe = "-", .line = 1, .type = TokenType::MINUS });
    mem.scanner.tokens.emplace_back(Token{ .lexMe = "*", .line = 1, .type = TokenType::STAR });

    u32 minusTokenIndex = mem.scanner.tokens.size() - 2;
    u32 starTokenIndex = mem.scanner.tokens.size() - 1;

    Expr u64Lit{ .value = 123, .exprType = ExprType_Literal, .literalType = LiteralType_U64 };
    mem.expressions.emplace_back(u64Lit);
    u32 u64ExpressionIndex = mem.expressions.size() - 1;

    Expr doubleLit{ .doubleValue = 45.67, .exprType = ExprType_Literal, .literalType = LiteralType_Double };
    mem.expressions.emplace_back(doubleLit);
    u32 doubleExpressionIndex = mem.expressions.size() - 1;

    Expr unary{ .tokenOperIndex = minusTokenIndex, .rightExprIndex = u64ExpressionIndex, .exprType = ExprType_Unary };
    mem.expressions.emplace_back(unary);
    u32 unaryExpressionIndex = mem.expressions.size() - 1;

    Expr grouping{ .rightExprIndex = doubleExpressionIndex, .exprType = ExprType_Grouping };
    mem.expressions.emplace_back(grouping);
    u32 groupingExpressionIndex = mem.expressions.size() - 1;
    
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
    printAst(mem, unary, s);
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