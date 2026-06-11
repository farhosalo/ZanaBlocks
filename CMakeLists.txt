cmake_minimum_required(VERSION 3.31.6)

project(ZanaBlocks VERSION 0.0.1)

find_package(PkgConfig REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Disable C++ Modules support, as it can cause issues with some dependencies and is not currently used in this project.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(CMAKE_STATIC_LIBRARY_PREFIX "")


# Compiler options 
if (MSVC)
    add_compile_options(
        /W4
        /WX
        /wd4996   # deprecated declarations
        /permissive- 
        /wd4251 
        /wd4127
    )
else()
    add_compile_options(
        -Wall -Wextra -Wpedantic -Werror
        -Wconversion -Wfloat-conversion
        -Wnon-virtual-dtor -Woverloaded-virtual
        -Wno-deprecated-declarations
        -Wno-gnu-zero-variadic-macro-arguments
    )
endif()

#Clang-Format
find_program(CLANG_FORMAT_BIN clang-format REQUIRED)

#Clang-Tidy
find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED)


#Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/Utilities
    ${CMAKE_BINARY_DIR}/Schema
    ${CMAKE_BINARY_DIR}/Schema/proto
    ${CMAKE_BINARY_DIR}/Interpreter
    ${CMAKE_SOURCE_DIR}/Interpreter/src
    ${CMAKE_SOURCE_DIR}/EspTools/src
    ${CMAKE_BINARY_DIR}/IDE
)

# Tell CMake where Homebrew lives — works for both Apple Silicon and Intel
if(APPLE)
    if(EXISTS "/opt/homebrew")
        link_directories(/opt/homebrew/lib)
        include_directories(/opt/homebrew/include)
    else()
        link_directories(/usr/local/lib)
        include_directories(/usr/local/include)
    endif()
endif()


file(GLOB_RECURSE LICENSE_FILES
    ${CMAKE_SOURCE_DIR}/LICENSES/*
)

install(FILES ${LICENSE_FILES}
    DESTINATION share/licenses
)

add_subdirectory(Schema)
add_subdirectory(Interpreter)
add_subdirectory(EspTools)
add_subdirectory(IDE)
