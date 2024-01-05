#ifndef KERNEL_H
#define KERNEL_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char bool;

#define true 1
#define false 0

// TODO: ensure has the same value as defined in GDT.
#define KERNEL_CODE_SEGMENT 0x08
#define INTERRUPT_TIMER 0
#define INTERRUPT_KEYBOARD 1

/**
 * Reads a single byte from the given port.
 */
extern u8 in(u16 port);

/**
 * Writes the given byte to the given port.
 */
extern void out(u16 port, u8 byte);

/**
 * Enables interrupts.
 */
extern void enable_interrupts();

/**
 * Halts the CPU (until next interrupt occurs).
 */
extern void halt();

/**
 * Initializes GDT.
 */
extern void init_gdt();

/**
 * Initializes IDT. Enables further usage of idt_set_interrupt_handler and idt_set_exception_handler.
 */
extern void init_idt();

/**
 * Sets the given function (pointed by base) to be executed when the given interrupt occurs.
 * See https://wiki.osdev.org/Interrupt_Descriptor_Table.
 */
extern void set_idt_entry(u8 interrupt, u32 base, u16 code_selector, u8 attributes);

/**
 * Initializes interrupt requests (irqs). Sets up proper handlers in IDT so that
 * hardware interrupts can be now handled by the OS code.
 */
extern void init_interrupt_handlers();

/**
 * Initializes exception handlers for CPU reserved interrupts.
 * This is necessary to ensure that in case problem happens (e.g. double fault)
 * it's properly handled and reported back.
 */
extern void init_exception_handlers();

/**
 * Register a handler for a given interrupt number.
 * The interrupt number must be between [0 and 15].
 */
extern void set_interrupt_handler(u32 interrupt, void (*handler)(u32 interrupt));

/**
 * Registers a handler for exceptions.
 */
extern void set_exception_handler(void (*handler)(u32 interrupt, u32 error, char *message));

#endif
