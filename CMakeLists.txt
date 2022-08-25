cmake_minimum_required(VERSION 3.10)

# single header libraries
project(shl)

#settings
file(GLOB_RECURSE HEADERS "src/*.h" "src/*.hpp")

set(CMAKE_CXX_STANDARD 17)

# install
install(FILES ${HEADERS} DESTINATION /usr/local/include/)

# tests
set(TEST_SRC_DIR ${CMAKE_SOURCE_DIR}/tests)
set(TEST_BIN_DIR ${CMAKE_BINARY_DIR}/tests)

find_package(t1 QUIET)
if(NOT DEFINED t1_DIR OR t1_DIR)
    add_test_directory(${TEST_SRC_DIR} ${CMAKE_SOURCE_DIR}/src)
    register_tests()
    
    configure_file(${TEST_SRC_DIR}/file_stream_binary_data.bin
                   ${TEST_BIN_DIR}/file_stream_binary_data.bin
                   COPYONLY)
    configure_file(${TEST_SRC_DIR}/file_stream_text_data.txt
                   ${TEST_BIN_DIR}/file_stream_text_data.txt
                   COPYONLY)
endif()
