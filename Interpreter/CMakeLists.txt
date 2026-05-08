project(Interpreter VERSION 0.0.1)

configure_file(src/${PROJECT_NAME}.h.in ${PROJECT_NAME}Info.h @ONLY)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(HEADERS
    src/Interpreter.h
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Info.h
)

add_library(${PROJECT_NAME}
    src/Interpreter.cpp
)
target_link_libraries(${PROJECT_NAME}
    Schema
)

file(GLOB_RECURSE ALL_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)
add_custom_target(${PROJECT_NAME}_format
    COMMAND ${CLANG_FORMAT_BIN} --dry-run --Werror --style=Google ${ALL_SOURCE_FILES}
)

#Format checking
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_format)

# Integrate clang-tidy with the build process of the main target
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}"
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(FILES ${HEADERS}
    DESTINATION include/${PROJECT_NAME}
)