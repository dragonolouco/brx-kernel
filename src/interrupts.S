.section .text
.code64

.extern brx_interrupt_dispatch

.global brx_isr_default
.global brx_isr_timer
.global brx_isr_keyboard
.global brx_isr_mouse

brx_isr_default:
    pushq $255
    jmp brx_isr_common

brx_isr_timer:
    pushq $32
    jmp brx_isr_common

brx_isr_keyboard:
    pushq $33
    jmp brx_isr_common

brx_isr_mouse:
    pushq $44
    jmp brx_isr_common

brx_isr_common:
    cld
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rbp
    pushq %rsi
    pushq %rdi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    movq 120(%rsp), %rdi
    call brx_interrupt_dispatch

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdi
    popq %rsi
    popq %rbp
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
    addq $8, %rsp
    iretq

.section .note.GNU-stack,"",@progbits
