global load_idt
load_idt:
    mov eax, [esp + 4]
    lidt [eax]
    ret


global load_gdt
load_gdt:
    mov eax, [esp + 4] ; passed as 1 argument
    lgdt [eax]
    mov ax, 0x10 ; data segment
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ; This is "far" jump. Ensures that CPU cleans up all the ongoing work
    ; and automatically sets CS to point to the code segment in the GDT.
    jmp 0x08:load_gdt_return ;code segment
load_gdt_return:
    ret


global out
out:
    mov al, [esp + 8] ; byte to write
    mov dx, [esp + 4] ; port
    out dx, al
    ret


global in
in:
    mov dx, [esp + 4] ; port
    in  al, dx        ; byte to return (convetion is to use ax to return values)
    ret


global enable_interrupts
enable_interrupts:
    sti
    ret


global halt
halt:
    hlt
    ret
