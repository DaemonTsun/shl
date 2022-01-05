cmake_minimum_required(VERSION 3.10)

# single header libraries
project(shl)

#settings
file(GLOB_RECURSE HEADERS "src/*.h" "src/*.hpp" "src/*.tcc")

set(CMAKE_CXX_STANDARD 17)

# install
install(FILES ${HEADERS} DESTINATION /usr/local/include/)

# tests
find_package(t1 QUIET)
if(NOT DEFINED t1_DIR OR t1_DIR)
    add_test_directory(${CMAKE_SOURCE_DIR}/tests ${CMAKE_SOURCE_DIR}/src)
    register_tests()
endif()
