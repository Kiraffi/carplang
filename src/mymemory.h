#pragma once

#include <vector>

#include "mytypes.h"
#include "scanner.h"

struct MyMemory
{
    Scanner scanner;
    std::vector<u8> scriptFileData;
};
