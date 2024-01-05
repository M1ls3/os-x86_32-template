#include "kernel.h"

#define INTERRUPT_GATE_TYPE_ATTRIBUTES 0x8E
#define MASTER_PIC_COMMAND_PORT 0x20
#define MASTER_PIC_DATA_PORT (MASTER_PIC_COMMAND_PORT + 1)
#define SLAVE_PIC_COMMAND_PORT 0xA0
#define SLAVE_PIC_DATA_PORT (SLAVE_PIC_COMMAND_PORT + 1)
#define MASTER_INTERRUPT_OFFSET 32
#define SLAVE_INTERRUPT_OFFSET (MASTER_INTERRUPT_OFFSET + 8)
#define END_OF_INTERRUPT_COMMAND 0x20
#define MODE_8086 0x01

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void *irq_handlers[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void set_interrupt_handler(u32 interrupt, void (*handler)(u32 interrupt)) {
    irq_handlers[interrupt] = handler;
}

void idt_set_interrupt_handler(u8 interrupt, void (*handler_ptr)()) {
    set_idt_entry(
        interrupt,
        (u32) handler_ptr, // cast is fine for 32bit system (Note: x32)
        KERNEL_CODE_SEGMENT,
        INTERRUPT_GATE_TYPE_ATTRIBUTES
    );
}

void init_interrupt_handlers() {
    // https://wiki.osdev.org/8259_PIC
    const u8 master_marks = in(MASTER_PIC_DATA_PORT);
    const u8 slave_marks  = in(SLAVE_PIC_DATA_PORT);
    out(MASTER_PIC_COMMAND_PORT, 0x11); // Init
    out(SLAVE_PIC_COMMAND_PORT, 0x11);  //  Init
    out(MASTER_PIC_DATA_PORT, MASTER_INTERRUPT_OFFSET); // Set offset
    out(SLAVE_PIC_DATA_PORT, SLAVE_INTERRUPT_OFFSET);   // Set offset
    out(MASTER_PIC_DATA_PORT, 0x04); // Tell master there is a slave connected on line 2
    out(SLAVE_PIC_DATA_PORT, 0x02);  // Tell slave it's "cascade identity", so it's line 2
    out(MASTER_PIC_DATA_PORT, MODE_8086);
    out(SLAVE_PIC_DATA_PORT, MODE_8086);
    out(MASTER_PIC_DATA_PORT, master_marks);
    out(SLAVE_PIC_DATA_PORT, slave_marks);

    idt_set_interrupt_handler(MASTER_INTERRUPT_OFFSET, irq0);
    idt_set_interrupt_handler(MASTER_INTERRUPT_OFFSET + 1, irq1);
    idt_set_interrupt_handler(MASTER_INTERRUPT_OFFSET + 2, irq2);
    idt_set_interrupt_handler(MASTER_INTERRUPT_OFFSET + 3, irq3);
    idt_set_interrupt_handler(MASTER_INTERRUPT_OFFSET + 4, irq4);
    idt_set_interrupt_handler(MASTER_INTERRUPT_OFFSET + 5, irq5);
    idt_set_interrupt_handler(MASTER_INTERRUPT_OFFSET + 6, irq6);
    idt_set_interrupt_handler(MASTER_INTERRUPT_OFFSET + 7, irq7);
    idt_set_interrupt_handler(SLAVE_INTERRUPT_OFFSET, irq8);
    idt_set_interrupt_handler(SLAVE_INTERRUPT_OFFSET + 1, irq9);
    idt_set_interrupt_handler(SLAVE_INTERRUPT_OFFSET + 2, irq10);
    idt_set_interrupt_handler(SLAVE_INTERRUPT_OFFSET + 3, irq11);
    idt_set_interrupt_handler(SLAVE_INTERRUPT_OFFSET + 4, irq12);
    idt_set_interrupt_handler(SLAVE_INTERRUPT_OFFSET + 5, irq13);
    idt_set_interrupt_handler(SLAVE_INTERRUPT_OFFSET + 6, irq14);
    idt_set_interrupt_handler(SLAVE_INTERRUPT_OFFSET + 7, irq15);
}

struct irq_stack_state {
    u32 gs, fs, es, ds;                         // pushed directly by common stub
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; // pusha in common stub
    u32 interrupt;                              // pushed by concrete routine
    u32 eip, cs, eflags, useresp, ss;           // automatically pushed by cpu
};

/**
 * Delegates execution of the interrupt to the registered routine (see irq_set_handler).
 * Will be called by ASM code whenever interrupt occurs (see ex_handlers.asm).
 */
void kernel_interrupt_handler(struct irq_stack_state *stack_ptr) {
    // Delegate handling to the function in case it's registered
    void (*handler)(u32 interrupt) = irq_handlers[stack_ptr->interrupt - MASTER_INTERRUPT_OFFSET];
    if (handler) {
        handler(stack_ptr->interrupt);
    }

    // send EOI to slave only if it's a slaves' interrupt
    if (stack_ptr->interrupt >= SLAVE_INTERRUPT_OFFSET) {
        out(SLAVE_PIC_COMMAND_PORT, END_OF_INTERRUPT_COMMAND);
    }

    // always send EOI to master
    out(MASTER_PIC_COMMAND_PORT, END_OF_INTERRUPT_COMMAND);
}
