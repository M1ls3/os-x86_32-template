global eh0
global eh1
global eh2
global eh3
global eh4
global eh5
global eh6
global eh7
global eh8
global eh9
global eh10
global eh11
global eh12
global eh13
global eh14
global eh15
global eh16
global eh17
global eh18
global eh19
global eh20
global eh21
global eh22
global eh23
global eh24
global eh25
global eh26
global eh27
global eh28
global eh29
global eh30
global eh31

eh0:
    cli
    push byte 0
    push byte 0
    jmp isr_common_stub

eh1:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub

eh2:
    cli
    push byte 0
    push byte 2
    jmp isr_common_stub

eh3:
    cli
    push byte 0
    push byte 3
    jmp isr_common_stub

eh4:
    cli
    push byte 0
    push byte 4
    jmp isr_common_stub

eh5:
    cli
    push byte 0
    push byte 5
    jmp isr_common_stub

eh6:
    cli
    push byte 0
    push byte 6
    jmp isr_common_stub

eh7:
    cli
    push byte 0
    push byte 7
    jmp isr_common_stub

eh8:
    cli
    push byte 8
    jmp isr_common_stub

eh9:
    cli
    push byte 0
    push byte 9
    jmp isr_common_stub

eh10:
    cli
    push byte 10
    jmp isr_common_stub

eh11:
    cli
    push byte 11
    jmp isr_common_stub

eh12:
    cli
    push byte 12
    jmp isr_common_stub

eh13:
    cli
    push byte 13
    jmp isr_common_stub

eh14:
    cli
    push byte 14
    jmp isr_common_stub

eh15:
    cli
    push byte 0
    push byte 15
    jmp isr_common_stub

eh16:
    cli
    push byte 0
    push byte 16
    jmp isr_common_stub

eh17:
    cli
    push byte 0
    push byte 17
    jmp isr_common_stub

eh18:
    cli
    push byte 0
    push byte 18
    jmp isr_common_stub

eh19:
    cli
    push byte 0
    push byte 19
    jmp isr_common_stub

eh20:
    cli
    push byte 0
    push byte 20
    jmp isr_common_stub

eh21:
    cli
    push byte 0
    push byte 21
    jmp isr_common_stub

eh22:
    cli
    push byte 0
    push byte 22
    jmp isr_common_stub

eh23:
    cli
    push byte 0
    push byte 23
    jmp isr_common_stub

eh24:
    cli
    push byte 0
    push byte 24
    jmp isr_common_stub

eh25:
    cli
    push byte 0
    push byte 25
    jmp isr_common_stub

eh26:
    cli
    push byte 0
    push byte 26
    jmp isr_common_stub

eh27:
    cli
    push byte 0
    push byte 27
    jmp isr_common_stub

eh28:
    cli
    push byte 0
    push byte 28
    jmp isr_common_stub

eh29:
    cli
    push byte 0
    push byte 29
    jmp isr_common_stub

eh30:
    cli
    push byte 0
    push byte 30
    jmp isr_common_stub

eh31:
    cli
    push byte 0
    push byte 31
    jmp isr_common_stub

extern kernel_exception_handler

isr_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10 ; ; kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, kernel_exception_handler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret
