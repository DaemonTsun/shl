
.intel_syntax noprefix

.text
    .globl _linux_thread_start

    _linux_thread_start:
        /*
        mov rsi, rdi
        mov rdi, 0x50f00
        mov rax, 56 // 56 is clone
        syscall

        ret
        */
        add rdi, 40     // clone_args offset within thread_stack_head
        mov rsi, 88     // size of clone_args
        mov rax, 435    // 435 is clone3
        syscall
        mov rdi, rsp

        ret
