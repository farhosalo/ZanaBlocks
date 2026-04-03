cmake_minimum_required(VERSION 4.2)

project(KidTech)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Clang-Format
find_program(CLANG_FORMAT_BIN clang-format REQUIRED)
