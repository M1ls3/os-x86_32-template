#include "kernel.h"

#define EXCEPTION_GATE_TYPE_ATTRIBUTES 0x8F

extern void eh0();
extern void eh1();
extern void eh2();
extern void eh3();
extern void eh4();
extern void eh5();
extern void eh6();
extern void eh7();
extern void eh8();
extern void eh9();
extern void eh10();
extern void eh11();
extern void eh12();
extern void eh13();
extern void eh14();
extern void eh15();
extern void eh16();
extern void eh17();
extern void eh18();
extern void eh19();
extern void eh20();
extern void eh21();
extern void eh22();
extern void eh23();
extern void eh24();
extern void eh25();
extern void eh26();
extern void eh27();
extern void eh28();
extern void eh29();
extern void eh30();
extern void eh31();

void idt_set_exception_handler(u8 interrupt, void (*handler_ptr)()) {
    set_idt_entry(
        interrupt,
        (u32) handler_ptr, // cast is fine for 32bit system (Note: x32)
        KERNEL_CODE_SEGMENT,
        EXCEPTION_GATE_TYPE_ATTRIBUTES
    );
}

void init_exception_handlers() {
    idt_set_exception_handler(0, eh0);
    idt_set_exception_handler(1, eh1);
    idt_set_exception_handler(2, eh2);
    idt_set_exception_handler(3, eh3);
    idt_set_exception_handler(4, eh4);
    idt_set_exception_handler(5, eh5);
    idt_set_exception_handler(6, eh6);
    idt_set_exception_handler(7, eh7);
    idt_set_exception_handler(8, eh8);
    idt_set_exception_handler(9, eh9);
    idt_set_exception_handler(10, eh10);
    idt_set_exception_handler(11, eh11);
    idt_set_exception_handler(12, eh12);
    idt_set_exception_handler(13, eh13);
    idt_set_exception_handler(14, eh14);
    idt_set_exception_handler(15, eh15);
    idt_set_exception_handler(16, eh16);
    idt_set_exception_handler(17, eh17);
    idt_set_exception_handler(18, eh18);
    idt_set_exception_handler(19, eh19);
    idt_set_exception_handler(20, eh20);
    idt_set_exception_handler(21, eh21);
    idt_set_exception_handler(22, eh22);
    idt_set_exception_handler(23, eh23);
    idt_set_exception_handler(24, eh24);
    idt_set_exception_handler(25, eh25);
    idt_set_exception_handler(26, eh26);
    idt_set_exception_handler(27, eh27);
    idt_set_exception_handler(28, eh28);
    idt_set_exception_handler(29, eh29);
    idt_set_exception_handler(30, eh30);
    idt_set_exception_handler(31, eh31);
}

// index in this array indicates number of the interrupt
char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

struct eh_stack_state {
    u32 gs, fs, es, ds;                         // pushed directly by common stub
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; // pusha in common stub
    u32 interrupt, error;                       // pushed by concrete routine
    u32 eip, cs, eflags, useresp, ss;           // automatically pushed by cpu
};

void (*custom_handler)(u32 interrupt, u32 error, const char *message) = 0;

void set_exception_handler(void (*handler)(u32 interrupt, u32 error, char *message)) {
    custom_handler = handler;
}

/**
 * Called from ASM. At this point there is nothing that happens in case of exception.
 */
void kernel_exception_handler(struct eh_stack_state *r) {
    if (r->interrupt < 32) {
        if (custom_handler != 0) {
            custom_handler(r->interrupt, r->error, exception_messages[r->interrupt]);
        }
        for (;;);
    }
}
