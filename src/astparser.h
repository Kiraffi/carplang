#pragma once

#include <string>

struct Expr;
struct MyMemory;

bool printAst(const MyMemory& mem, const Expr& expr, std::string& printStr);

bool ast_generate(MyMemory& mem);

