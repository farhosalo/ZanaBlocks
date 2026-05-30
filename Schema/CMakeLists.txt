project(Schema VERSION 0.0.1)
list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
find_package(Protobuf CONFIG REQUIRED)

configure_file(${PROJECT_NAME}.h.in ${PROJECT_NAME}Info.h @ONLY)

set (PROTO_FILES
    proto/Schema.proto
)

add_library(${PROJECT_NAME}
    STATIC
    ${PROTO_FILES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    protobuf::libprotobuf
)

# suppress all warnings on generated code
set_target_properties(${PROJECT_NAME} PROPERTIES
    COMPILE_OPTIONS "-w"   
)

protobuf_generate(
    TARGET Schema
)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)


file(GLOB_RECURSE HEADERS
    ${CMAKE_CURRENT_BINARY_DIR}/*.h
)

install(FILES ${HEADERS}
    DESTINATION include/${PROJECT_NAME}
)
