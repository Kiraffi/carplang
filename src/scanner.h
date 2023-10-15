#pragma once

#include <vector>

#include "mytypes.h"
#include "token.h"

struct MyMemory;

struct Scanner
{
    MyMemory& mem;
    const u8* src;
    i32 srcLen;
    i32 pos;
    i32 start;
    i32 line;
    bool hasErrors;
};

bool scanner_run(MyMemory& mem);

