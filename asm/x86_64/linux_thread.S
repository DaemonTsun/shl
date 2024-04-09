Clone_Args_Offset   = 72
Clone_Args_Size     = 88
SYS_clone           = 56
SYS_clone3          = 435
ERRNO_Not_implemented = -38

.intel_syntax noprefix

.text
    .globl _linux_thread_start

    _linux_thread_start:
        /* clone3 interface:
           rax  SYS_clone3
           rdi  pointer to clone args
           rsi  clone args size
        */
        add rdi, Clone_Args_Offset
        mov rsi, Clone_Args_Size     // size of clone_args
        mov rax, SYS_clone3    // 435 is clone3
        syscall

        cmp rax, ERRNO_Not_implemented
        jne _linux_thread_start_end
        /* check return value for -38 (Function not implemented) and
           attempt to run the clone() system call instead.

           clone interface:
           rax  SYS_clone
           rdi  flags
           rsi  stack high end
           rdx  parent tid pointer
           r10  child tid pointer
           r8   tls
        */

        mov rsi, rdi
        sub rsi, Clone_Args_Offset         // move back to head
        mov rdi, [rsi + Clone_Args_Offset] // flags
        mov rdx, [rsi + Clone_Args_Offset + 24] // parent tid
        mov r10, [rsi + Clone_Args_Offset + 16] // child tid
        mov r8,  [rsi + Clone_Args_Offset + 56] // tls
        mov rax, SYS_clone
        syscall

    _linux_thread_start_end:
        mov rdi, rsp

        ret
