cmake_minimum_required(VERSION 3.10)
find_package(better REQUIRED)

project(shl VERSION 0.5)
get_version_target(shl_TARGET ${PROJECT_NAME})

if(TARGET ${shl_TARGET})
    return()
endif()

#settings
set(CMAKE_CXX_STANDARD 20)

set(shl_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")
find_sources(shl_SOURCES "${shl_SOURCE_DIR}")
find_headers(shl_HEADERS "${shl_SOURCE_DIR}")

# library
add_library(${shl_TARGET} SHARED)
target_include_directories(${shl_TARGET} PUBLIC "${shl_SOURCE_DIR}")
target_sources(${shl_TARGET} PUBLIC ${shl_HEADERS} ${shl_SOURCES})
set_property(TARGET ${shl_TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)

if (NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # included file
    set(shl_VERSION ${shl_VERSION} PARENT_SCOPE)
    set(shl_TARGET ${shl_TARGET} PARENT_SCOPE)
    set(shl_SOURCE_DIR "${shl_SOURCE_DIR}" PARENT_SCOPE)
    set(shl_SOURCES ${shl_SOURCES} PARENT_SCOPE)
    set(shl_HEADERS ${shl_HEADERS} PARENT_SCOPE)
else()
    # not included file

    # install
    install_library(TARGET ${shl_TARGET} HEADERS ${shl_HEADERS})

    # tests
    set(TEST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
    set(TEST_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/tests)

    find_package(t1 QUIET)
    if(NOT DEFINED t1_DIR OR t1_DIR)
        add_test_directory("${TEST_SOURCE_DIR}" INCLUDE_DIRS "${shl_SOURCE_DIR}" LIBRARIES ${shl_TARGET} pthread)
        register_tests()
        
        configure_file(${TEST_SOURCE_DIR}/file_stream_binary_data.bin
                       ${TEST_BINARY_DIR}/file_stream_binary_data.bin
                       COPYONLY)
        configure_file(${TEST_SOURCE_DIR}/file_stream_text_data.txt
                       ${TEST_BINARY_DIR}/file_stream_text_data.txt
                       COPYONLY)
    endif()
endif()
