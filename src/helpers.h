#pragma once

#include "mymemory.h"
#include "mytypes.h"

#include <string>

u32 addToken(MyMemory& mem, const Token& token);
u32 addExpr(MyMemory& mem, const Expr& expr);
u32 addString(MyMemory& mem, const std::string& str);

const Token& getTokenOper(const MyMemory& mem, const Expr& expr);
const Expr& getLeftExprValue(const MyMemory& mem, const Expr& expr);
const Expr& getRightExpr(const MyMemory& mem, const Expr& expr);

bool checkNumber(const ExprValue& exprValue);
bool checkString(const ExprValue& exprValue);

double getDouble(const ExprValue& exprValue);
i64 getInt(const ExprValue& exprValue);

std::string stringify(const MyMemory& mem, const ExprValue& exprValue);
