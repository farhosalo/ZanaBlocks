cmake_minimum_required(VERSION 3.31.6)

project(ZanaBlocks VERSION 0.0.1)

find_package(PkgConfig REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_STATIC_LIBRARY_PREFIX "")
add_compile_definitions(
    QT_NO_KEYWORDS          # Avoid signal/slot macro conflicts
)

if (NOT MSVC)
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
        -Wno-deprecated-declarations
    )
else()
    add_compile_options(/W4 /permissive- /wd4251 /wd4127)
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
