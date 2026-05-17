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
set(PORT "LINUX" CACHE STRING "" FORCE)

FetchContent_GetProperties(esp_serial_flasher)


FetchContent_MakeAvailable(esp_serial_flasher)

# ── fix missing high baud rate constants ──────────────────────────────────────
include(CheckSymbolExists)
check_symbol_exists(B460800  "termios.h" HAVE_B460800)
check_symbol_exists(B921600  "termios.h" HAVE_B921600)
check_symbol_exists(B1000000 "termios.h" HAVE_B1000000)
check_symbol_exists(B500000 "termios.h" HAVE_B500000)
check_symbol_exists(B576000 "termios.h" HAVE_B576000)
check_symbol_exists(B1152000 "termios.h" HAVE_B1152000)
check_symbol_exists(B1500000 "termios.h" HAVE_B1500000)
check_symbol_exists(B2000000 "termios.h" HAVE_B2000000)
check_symbol_exists(B2500000 "termios.h" HAVE_B2500000)
check_symbol_exists(B3000000 "termios.h" HAVE_B3000000)
check_symbol_exists(B3500000 "termios.h" HAVE_B3500000)
check_symbol_exists(B4000000 "termios.h" HAVE_B4000000)

if( HAVE_B460800)
    set(FAST_BUAD 460800)
elseif(HAVE_B921600) 
    set(FAST_BUAD 921600)
else() 
    set(FAST_BUAD 230400)
endif()

if(NOT HAVE_B460800)
    target_compile_definitions(flasher PRIVATE B460800=B230400)
endif()
if(NOT HAVE_B921600)
    target_compile_definitions(flasher PRIVATE B921600=B230400)
endif()
if(NOT HAVE_B1000000)
    target_compile_definitions(flasher PRIVATE B1000000=B230400)
endif()
if(NOT HAVE_B500000)
    target_compile_definitions(flasher PRIVATE B500000=B230400)
endif()
if(NOT HAVE_B576000) 
    target_compile_definitions(flasher PRIVATE B576000=B230400) 
endif()
if(NOT HAVE_B576000)
    target_compile_definitions(flasher PRIVATE B1152000=B230400) 
endif()
if(NOT HAVE_B576000)
 target_compile_definitions(flasher PRIVATE B1500000=B230400) 
endif()
if(NOT HAVE_B576000)
 target_compile_definitions(flasher PRIVATE B2000000=B230400) 
endif()
if(NOT HAVE_B576000)
 target_compile_definitions(flasher PRIVATE B2500000=B230400) 
endif()
if(NOT HAVE_B576000)
 target_compile_definitions(flasher PRIVATE B3000000=B230400) 
endif()
if(NOT HAVE_B576000)
 target_compile_definitions(flasher PRIVATE B3500000=B230400) 
endif()
if(NOT HAVE_B576000)
 target_compile_definitions(flasher PRIVATE B4000000=B230400) 
endif()
 
# ---------------------------------------------------------------------------
set(HEADERS
    src/EspTools.h
    src/ReplClient.h
    src/EspFlasher.h
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Info.h
)

add_library(${PROJECT_NAME}
    src/EspTools.cpp
    src/ReplClient.cpp
    src/EspFlasher.cpp
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