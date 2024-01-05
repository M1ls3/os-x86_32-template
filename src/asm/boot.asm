[BITS 32]

; This is called by grub. Grub has switched us to the protected mode (32bit)
; and also disabled intrrupts. Grub has set up it's own GDT do execute the
; switch to the protected mode. If boot is successful then we must have
; 0x2BADB002 value set in the eax. Grub will call whatever entrypoint we
; specify in the ELF file. We configure linker to make this "start" such entrypoint.
global start
start:
    mov esp, stack_top
    jmp entry_kernel

; Multiboot header must be 4bytes aligned as per specification
; otherwise it might not be spotted by grub.
; The alignment means that this section will be aligned in respect to
; the previous section so that it STARTS at 4 bytes multiple address.
; 'hexdump' can be used to compare outputs with alignment and without.
ALIGN 4
multiboot:
    ; Multiboot values, not part of memory layout yet.
    MULTIBOOT_PAGE_ALIGN	equ 1 << 0
    MULTIBOOT_MEMORY_INFO	equ 1 << 1
    MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
    MULTIBOOT_HEADER_FLAGS	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
    MULTIBOOT_CHECKSUM	    equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

    ; Mutiboot header.
    ; The actual values will be the ones specified above.
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_CHECKSUM

    ; Define 5 dd (4 bytes) according to the spec for ELF file.
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0

entry_kernel:
    call run_checks
    extern kernel_entry
    call kernel_entry
    jmp $

%include "src/asm/checks.asm"

SECTION .bss
    resb 8192 ; Reserves 8kb of memory in BSS section for kernel stack.
stack_top:
