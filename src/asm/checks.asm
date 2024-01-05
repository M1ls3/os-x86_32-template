; Error codes:
; M - indicates absence of expected multiboot value in aex.

run_checks:
    call check_multiboot
    ret

; Spec - https://www.gnu.org/software/grub/manual/multiboot/multiboot.html
; We can check a few things, but at least worth checking that the EAX value is correct.
check_multiboot:
    cmp eax, 0x2BADB002
    jne error_multiboot
    ret

error_multiboot:
    mov al, "M"
    jmp print_error_and_halt

; Print an error code that is stored in al.
; The error code can be passed using nasm ASCII covertion,
; like mov al, "X".
print_error_and_halt:
    RED_TEXT equ 0xc
    VGA_BUF  equ 0xb8000

    mov byte [VGA_BUF], "E"
    mov byte [VGA_BUF + 1], RED_TEXT
    mov byte [VGA_BUF + 2], "R"
    mov byte [VGA_BUF + 3], RED_TEXT
    mov byte [VGA_BUF + 4], "R"
    mov byte [VGA_BUF + 5], RED_TEXT
    mov byte [VGA_BUF + 6], "O"
    mov byte [VGA_BUF + 7], RED_TEXT
    mov byte [VGA_BUF + 8], "R"
    mov byte [VGA_BUF + 9], RED_TEXT
    mov byte [VGA_BUF + 10], " "
    mov byte [VGA_BUF + 11], RED_TEXT
    mov byte [VGA_BUF + 12], al
    mov byte [VGA_BUF + 13], RED_TEXT
    hlt
