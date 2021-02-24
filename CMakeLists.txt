cmake_minimum_required(VERSION 3.5)

project(kanlang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(kanlang_parser kan/parser.cpp
        kan/parser.hpp kan/semantic.cpp kan/semantic.hpp
        kan/kan_semantic.cpp kan/kan_semantic.hpp)

add_library(kanlang_executor kan/executor/compiler.cpp
        kan/executor/compiler.hpp
        kan/executor/types/types.cpp
        kan/executor/types/types.hpp
        kan/executor/executor.cpp kan/executor/executor.hpp
        kan/executor/statements_class.hpp kan/executor/statements_class.cpp)

add_executable(kan main.cpp)

target_link_libraries(kanlang_executor kanlang_parser)

target_link_libraries(kan kanlang_parser
        kanlang_executor)

