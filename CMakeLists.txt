cmake_minimum_required(VERSION 3.26)
project(carplang)

set(CMAKE_CXX_STANDARD 20)


add_executable(carplang src/main.cpp
        "src/errors.cpp"
        "src/errors.h"

        "src/token.cpp"
        "src/token.h"
        "src/mymemory.h"
        "src/mytypes.h"

        "src/scanner.h"
        "src/scanner.cpp"

        "src/astparser.h"
        "src/astparser.cpp"
        "src/expr.h"

        "src/interpreter.h"
        "src/interpreter.cpp"
        "src/helpers.h"
        "src/helpers.cpp"
        "src/statement.h"
        "src/environment.h"
        "src/environment.cpp"
        "src/block.h"
)

