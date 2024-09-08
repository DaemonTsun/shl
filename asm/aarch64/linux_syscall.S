
.text
    .globl linux_syscall
    .globl linux_syscall1, linux_syscall2, linux_syscall3, linux_syscall4, linux_syscall5, linux_syscall6

linux_syscall:
        mov     x8, x0
        svc     #0
        ret
linux_syscall1:
        mov     x8, x0
        mov     x0, x1
        svc     #0
        ret
linux_syscall2:
        mov     x8, x0
        mov     x0, x1
        mov     x1, x2
        svc     #0
        ret
linux_syscall3:
        mov     x8, x0
        mov     x0, x1
        mov     x1, x2
        mov     x2, x3
        svc     #0
        ret
linux_syscall4:
        mov     x8, x0
        mov     x0, x1
        mov     x1, x2
        mov     x2, x3
        mov     x3, x4
        svc     #0
        ret
linux_syscall5:
        mov     x8, x0
        mov     x0, x1
        mov     x1, x2
        mov     x2, x3
        mov     x3, x4
        mov     x4, x5
        svc     #0
        ret
linux_syscall6:
        mov     x8, x0
        mov     x0, x1
        mov     x1, x2
        mov     x2, x3
        mov     x3, x4
        mov     x4, x5
        mov     x5, x6
        svc     #0
        ret
