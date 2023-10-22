#pragma once

#include "mymemory.h"
#include "mytypes.h"

#include <string>

u32 addToken(MyMemory& mem, const Token& token);
u32 addExpr(MyMemory& mem, const Expr& expr);
u32 addString(MyMemory& mem, const std::string& str);
u32 addStatement(MyMemory& mem, const Statement& statement);

const Token& getTokenOper(const MyMemory& mem, const Expr& expr);
const Expr& getLeftExprValue(const MyMemory& mem, const Expr& expr);
const Expr& getRightExpr(const MyMemory& mem, const Expr& expr);

bool checkNumber(const ExprValue& exprValue);
bool checkString(const ExprValue& exprValue);

double getDouble(const ExprValue& exprValue);
i64 getInt(const ExprValue& exprValue);

std::string stringify(const MyMemory& mem, const ExprValue& exprValue);

const ExprValue& getConstValue(const MyMemory& mem, u32 stringIndex);
const ExprValue& getConstValue(const MyMemory& mem, const ExprValue& exprValue);
const ExprValue& getConstValue(const MyMemory& mem, const Token& token);
ExprValue& getMutableValue(MyMemory& mem, u32 stringIndex);
ExprValue& getMutableValue(MyMemory& mem, const ExprValue& exprValue);
ExprValue& getMutableValue(MyMemory& mem, const Token& token);
void defineVariable(MyMemory& mem, const std::string& name, const ExprValue& value);
