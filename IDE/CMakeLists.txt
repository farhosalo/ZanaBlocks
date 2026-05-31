set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Concurrent)

configure_file(src/${PROJECT_NAME}.h.in ${PROJECT_NAME}Info.h @ONLY)

add_library(lib${PROJECT_NAME}
    src/MainWindow.cpp
    src/LogOutput.cpp
    src/Node.cpp
    src/NodeConnector.cpp
    src/Scene.cpp
    src/NodePort.cpp
    src/View.cpp
    src/ComponentButton.cpp
    src/LoopNode.cpp
    src/PrintNode.cpp
    src/SleepNode.cpp
    src/LedNode.cpp 
    src/NodeSettings.cpp
    src/IdeSettings.cpp
    src/LicenseDialog.cpp
    src/AboutDialog.cpp
    src/PwmNode.cpp
)

target_link_libraries(lib${PROJECT_NAME}
    Qt6::Core 
    Qt6::Widgets
    Qt6::Concurrent
    Schema
    Interpreter 
    EspTools
)

add_executable(${PROJECT_NAME}
    src/main.cpp
)
target_link_libraries(${PROJECT_NAME}
    lib${PROJECT_NAME}
)
set_target_properties(lib${PROJECT_NAME} PROPERTIES
    CXX_CLANG_TIDY "${CLANG_TIDY_EXE};--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/.*" 
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
    CXX_CLANG_TIDY "${CLANG_TIDY_EXE};--header-filter=${CMAKE_CURRENT_SOURCE_DIR}/.*" 
)
qt_add_resources(${PROJECT_NAME} resources
    PREFIX "/"
    FILES
        assets/icon.png
)

# Install the executable
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Install headers
set (HEADERS
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Info.h
    src/MainWindow.h
    src/LogOutput.h
    src/Node.h
    src/NodePort.h
    src/NodeConnector.h
    src/Scene.h
    src/View.h
    src/ComponentButton.h
    src/LoopNode.h
    src/PrintNode.h
    src/SleepNode.h
    src/LedNode.h
    src/NodeSettings.h
    src/IdeSettings.h
    src/LicenseDialog.h
    src/AboutDialog.h
    src/PwmNode.h
)
install(FILES ${HEADERS}
    DESTINATION include/${PROJECT_NAME}
)