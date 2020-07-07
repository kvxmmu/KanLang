cmake_minimum_required(VERSION 3.5)

project(kanlang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(kanlang_compiler kan/compiler.cpp kan/compiler.hpp
       kan/lexer.cpp kan/lexer.hpp
       kan/traceback.cpp kan/traceback.hpp
       kan/parser.cpp kan/parser.hpp
       kan/types.cpp kan/types.hpp)

add_executable(kanlang main.cpp)

target_link_libraries(kanlang kanlang_compiler)
include_directories(.)
