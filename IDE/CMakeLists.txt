project(IDE VERSION 0.0.1)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

configure_file(src/${PROJECT_NAME}.h.in ${PROJECT_NAME}Info.h @ONLY)

add_library(lib${PROJECT_NAME}
    src/MainWindow.cpp
    src/ReplClient.cpp
    src/LogOutput.cpp
    src/Node.cpp
    src/NodeConnector.cpp
    src/Scene.cpp
    src/NodePort.cpp
    src/View.cpp
    src/ComponentButton.cpp
    src/ChildLoop.cpp
    src/Print.cpp
)

target_link_libraries(lib${PROJECT_NAME}
    Qt6::Core 
    Qt6::Widgets
    serialport
)
add_executable(${PROJECT_NAME}
    src/main.cpp
)
target_link_libraries(${PROJECT_NAME}
    lib${PROJECT_NAME}
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

# Install the executable
install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DIR}/bin)

# Install headers
set (HEADERS
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Info.h
    src/MainWindow.h
    src/ReplClient.h
    src/LogOutput.h
    src/Node.h
    src/NodePort.h
    src/NodeConnector.h
    src/Scene.h
    src/View.h
    src/ComponentButton.h
    src/MainLoop.h
    src/ChildLoop.h
    src/Print.h
)
install(FILES ${HEADERS}
    DESTINATION ${INSTALL_DIR}/include/${PROJECT_NAME}
)