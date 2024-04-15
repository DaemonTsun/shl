
if (Windows)
    return()
endif()

set(ASM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/asm")
set(ASM_ARCHITECTURE_DIR "${ASM_DIR}/${CMAKE_SYSTEM_PROCESSOR}")

if (NOT IS_DIRECTORY "${ASM_ARCHITECTURE_DIR}")
    message(FATAL_ERROR "Architecture ${CMAKE_SYSTEM_PROCESSOR} not supported")
endif()


target_sources(shl-0.9.3 PRIVATE
    "${ASM_ARCHITECTURE_DIR}/linux_syscall.S"
    "${ASM_ARCHITECTURE_DIR}/linux_thread.S"
    )
