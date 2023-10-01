cmake_minimum_required(VERSION 3.26)
project(carplang)

set(CMAKE_CXX_STANDARD 17)


add_executable(carplang src/main.cpp
        src/errors.cpp
        src/errors.h

        src/tokens.cpp
        src/tokens.h
        src/types.h
)

