cmake_minimum_required(VERSION 3.5)

project(kanlang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(kanlang_parser kan/parser.cpp
        kan/parser.hpp kan/semantic.cpp kan/semantic.hpp)

add_executable(kan main.cpp)

target_link_libraries(kan kanlang_parser)

