Clone_Args_Offset   = 72
Clone_Args_Size     = 88
SYS_clone           = 120
SYS_clone3          = 435
ERRNO_Not_implemented = -38

.text
    .globl _linux_thread_start

    _linux_thread_start:
        /* clone3 interface:
           x8  SYS_clone3
           x0  pointer to clone args
           x1  clone args size
        */
        add x0, x0, Clone_Args_Offset
        mov x1, Clone_Args_Size
        mov x8, SYS_clone3
        svc #0

        cmp x0, ERRNO_Not_implemented
        b.ne _linux_thread_start_end
        /* check return value for -38 (Function not implemented) and
           attempt to run the clone() system call instead.

           clone interface:
           x8  SYS_clone
           x0  flags
           x1  stack high end
           x2  parent tid pointer
           x3  child tid pointer
           x4   tls
        */

        mov x1, x0
        sub x1, x1, Clone_Args_Offset         // move back to head
        ldr x0, [x1, Clone_Args_Offset] // flags
        ldr x2, [x1, Clone_Args_Offset + 24] // parent tid
        ldr x3, [x1, Clone_Args_Offset + 16] // child tid
        ldr x4, [x1, Clone_Args_Offset + 56] // tls
        mov x8, SYS_clone
        svc #0

    _linux_thread_start_end:
        mov x0, sp

        ret
