project(Schema VERSION 0.0.1)

find_package(Protobuf CONFIG)

configure_file(${PROJECT_NAME}.h.in ${PROJECT_NAME}Info.h @ONLY)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
set (PROTO_FILES
    proto/Schema.proto
)

add_library(${PROJECT_NAME})

# Attach proto files to the target directly
target_sources(${PROJECT_NAME} PRIVATE ${PROTO_FILES})

protobuf_generate(
    LANGUAGE CPP 
    TARGET ${PROJECT_NAME}
    IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/proto
    PROTOC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}
    PROTOS ${PROTO_FILES}
)

target_link_libraries(${PROJECT_NAME}
    protobuf::libprotobuf  
)   

install(TARGETS ${PROJECT_NAME} DESTINATION ${INSTALL_DIR}/lib)
