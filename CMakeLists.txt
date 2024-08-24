cmake_minimum_required(VERSION 3.10)
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ext/better-cmake/cmake/betterConfig.cmake")
    execute_process(COMMAND git submodule update --init "${CMAKE_CURRENT_SOURCE_DIR}/ext/better-cmake" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}") 
endif()
find_package(better REQUIRED PATHS ext/better-cmake/cmake)

project(shl CXX ASM)
project_author("DaemonTsun")

include(cmake/ASM.cmake)

# library
add_lib(shl STATIC
    VERSION 0.10.0
    SOURCES_DIR "${ROOT}/src" EXCLUDING "${ROOT}/src/shl/impl"
    SOURCES @Linux   "${ROOT}/src/shl/impl/linux"
                     ${ASM_LINUX_SOURCES}
            @Windows "${ROOT}/src/shl/impl/windows"
    GENERATE_TARGET_HEADER "${ROOT}/src/shl/shl_info.hpp"
    CPP_VERSION 20
    CPP_WARNINGS ALL EXTRA SANE FATAL
                 @GNU -Werror
    LIBRARIES
        @Windows mincore.lib # for VirtualAlloc2 in ring_buffer.cpp
    TESTS "${ROOT}/tests"
    )

exit_if_included()

if (DEFINED Tests)
    copy_files(FILES ${ROOT}/tests/file_stream_binary_data.bin
                     ${ROOT}/tests/file_stream_text_data.txt
                     ${ROOT}/tests/hello_world.txt
               DESTINATION "${ROOT_BIN}"
               BASE "${ROOT}")
endif()
