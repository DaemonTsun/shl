cmake_minimum_required(VERSION 3.10)
add_compile_options (-fdiagnostics-color=always)

#settings
set(SHL_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")
file(GLOB_RECURSE SHL_SOURCES "${SHL_SRC_DIR}/*.c" "${SHL_SRC_DIR}/*.cpp")
file(GLOB_RECURSE SHL_HEADERS "${SHL_SRC_DIR}/*.h" "${SHL_SRC_DIR}/*.hpp")

if (NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # included file
    set(SHL_SRC_DIR "${SHL_SRC_DIR}" PARENT_SCOPE)
    set(SHL_SOURCES ${SHL_SOURCES} PARENT_SCOPE)
    set(SHL_HEADERS ${SHL_HEADERS} PARENT_SCOPE)
else()
    project(shl)

    set(CMAKE_CXX_STANDARD 20)

    include_directories("${SHL_SRC_DIR}")

    add_library(shl SHARED)
    target_sources(shl PUBLIC ${SHL_HEADERS} ${SHL_SOURCES})
    set_property(TARGET shl PROPERTY POSITION_INDEPENDENT_CODE ON)


    # install
    install(FILES ${SHL_HEADERS} DESTINATION /usr/local/include/)
    install(FILES ${SHL_HEADERS} DESTINATION "include/${PROJECT_NAME}")
    install(TARGETS "${PROJECT_NAME}"
            RUNTIME DESTINATION "bin"
            LIBRARY DESTINATION "lib"
            ARCHIVE DESTINATION "lib/${PROJECT_NAME}")

    # tests
    set(TEST_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
    set(TEST_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/tests)

    find_package(t1 QUIET)
    if(NOT DEFINED t1_DIR OR t1_DIR)
        add_test_directory("${TEST_SRC_DIR}" INCLUDE_DIRS "${SHL_SRC_DIR}" LIBRARIES shl pthread)
        register_tests()
        
        configure_file(${TEST_SRC_DIR}/file_stream_binary_data.bin
                       ${TEST_BIN_DIR}/file_stream_binary_data.bin
                       COPYONLY)
        configure_file(${TEST_SRC_DIR}/file_stream_text_data.txt
                       ${TEST_BIN_DIR}/file_stream_text_data.txt
                       COPYONLY)
    endif()

    add_subdirectory("${CMAKE_SOURCE_DIR}/demos")
endif()
