cmake_minimum_required(VERSION 3.10)
find_package(better REQUIRED PATHS ext/better-cmake/cmake)

project(shl)
project_author("DaemonTsun")

set(TEST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(TEST_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/tests)

# library
add_lib(shl STATIC
    VERSION 0.6
    SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/"
    GENERATE_TARGET_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/src/shl/shl_info.hpp"
    CPP_VERSION 20
    CPP_WARNINGS ALL
    TESTS "${TEST_SOURCE_DIR}"
    )

find_package(t1 QUIET)
if (t1_FOUND)
    configure_file(${TEST_SOURCE_DIR}/file_stream_binary_data.bin
                   ${TEST_BINARY_DIR}/file_stream_binary_data.bin
                   COPYONLY)
    configure_file(${TEST_SOURCE_DIR}/file_stream_text_data.txt
                   ${TEST_BINARY_DIR}/file_stream_text_data.txt
                   COPYONLY)
endif()
