.intel_syntax noprefix

.text
    .globl linux_syscall
    .globl linux_syscall1, linux_syscall2, linux_syscall3, linux_syscall4, linux_syscall5, linux_syscall6

    linux_syscall:
        mov rax,rdi
        syscall
        ret

    linux_syscall1:
        mov rax,rdi
        mov rdi,rsi
        syscall
        ret

    linux_syscall2:
        mov rax,rdi
        mov rdi,rsi
        mov rsi,rdx
        syscall
        ret

    linux_syscall3:
        mov rax,rdi
        mov rdi,rsi
        mov rsi,rdx
        mov rdx,rcx
        syscall
        ret

    linux_syscall4:
        mov rax,rdi
        mov rdi,rsi
        mov rsi,rdx
        mov rdx,rcx
        mov r10,r8
        syscall
        ret

    linux_syscall5:
        mov rax,rdi
        mov rdi,rsi
        mov rsi,rdx
        mov rdx,rcx
        mov r10,r8
        mov r8,r9
        syscall
        ret

    linux_syscall6:
        push rbp
        mov rbp, rsp
        mov rax,rdi
        mov rdi,rsi
        mov rsi,rdx
        mov rdx,rcx
        mov r10,r8
        mov r8,r9
        mov r9, qword ptr [rsp+0x10]
        syscall
        pop rbp
        ret
