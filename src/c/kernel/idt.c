#include "kernel.h"

#define IDT_SIZE 512

/**
 * Loads IDT pointing to the memory address given.
 */
extern void load_idt(void *idt_address);

struct idt_entry {
    u16 base01;
    u16 gdt_code_segment_selector;
    u8 reserved;
    u8 field5_attributes;
    u16 base23;
} __attribute__((packed));

struct idt_pointer {
    u16 limit;
    u32 base;
} __attribute__((packed));

struct idt_entry idt[IDT_SIZE];
struct idt_pointer idt_ptr;

void set_idt_entry(u8 interrupt, u32 base, u16 code_selector, u8 attributes) {
    idt[interrupt].base01 = (base & 0xFFFF);
    idt[interrupt].base23 = (base >> 16) & 0xFFFF;
    idt[interrupt].gdt_code_segment_selector = code_selector;
    idt[interrupt].reserved = 0;
    idt[interrupt].field5_attributes = attributes;
}

void zero_memory(void *dest, u32 count) {
    u8 *temp = (u8 *) dest;
    for (; count != 0; count--) *temp++ = 0;
}

void init_idt() {
    idt_ptr.limit = (sizeof(struct idt_entry) * IDT_SIZE) - 1;
    idt_ptr.base = (u32) &idt; // (Note: x32)
    zero_memory(&idt, sizeof(struct idt_entry) * IDT_SIZE);
    load_idt(&idt_ptr);
}
