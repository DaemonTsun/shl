
.intel_syntax noprefix

.text
    .globl _linux_thread_start

    _linux_thread_start:
        add rdi, 64     // clone_args offset within thread_stack_head
        mov rsi, 88     // size of clone_args
        mov rax, 435    // 435 is clone3
        syscall
        mov rdi, rsp

        ret
