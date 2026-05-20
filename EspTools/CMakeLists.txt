project(EspTools VERSION 0.0.1)

configure_file(src/${PROJECT_NAME}.h.in ${PROJECT_NAME}Info.h @ONLY)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
# ---------------------------------------------------------------------------
# esp-serial-flasher  (Apache 2.0)
# ---------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    esp_serial_flasher
    GIT_REPOSITORY https://github.com/espressif/esp-serial-flasher.git
    GIT_TAG        f1cccac82a41f6d494d953359d5ca2f5d70a9b12 #master
)

# Built-in Linux/macOS port — uses termios, no extra dependencies
set(PORT "USER_DEFINED" CACHE STRING "" FORCE)

FetchContent_GetProperties(esp_serial_flasher)


FetchContent_MakeAvailable(esp_serial_flasher)

set(HEADERS
    src/EspTools.h
    src/ReplClient.h
    src/EspFlasher.h
    src/SerialPort.h
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Info.h
)

add_library(${PROJECT_NAME}
    src/EspTools.cpp
    src/ReplClient.cpp
    src/EspFlasher.cpp
    src/SerialPort.cpp
)

if (WIN32)
    find_package(libserialport CONFIG REQUIRED)

    target_link_libraries(${PROJECT_NAME}
        libserialport::libserialport
        flasher
    )

else()
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(SERIALPORT REQUIRED libserialport)

    target_include_directories(${PROJECT_NAME} PRIVATE
        ${SERIALPORT_INCLUDE_DIRS}
        esp-serial-flasher
    )
    target_link_directories(${PROJECT_NAME} PRIVATE
        ${SERIALPORT_LIBRARY_DIRS}
    )
    target_link_libraries(${PROJECT_NAME}
        ${SERIALPORT_LIBRARIES}
        flasher
    )

endif()
target_include_directories(${PROJECT_NAME} PRIVATE
    ${esp_serial_flasher_SOURCE_DIR}/port   # for linux_port.h
    ${esp_serial_flasher_SOURCE_DIR}/private_include   # for linux_port.h
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

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)
install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})