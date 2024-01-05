#include "../../kernel/kernel.h"
#include "timer.h"

void (*custom_timer_interrupt_handler)() = 0;

void timer_handler(__attribute__((unused)) u32 interrupt) {
    if (custom_timer_interrupt_handler != 0) {
        custom_timer_interrupt_handler();
    }
}

void register_timer_interrupt_handler() {
    set_interrupt_handler(INTERRUPT_TIMER, timer_handler);
}

extern void timer_set_handler(void (*handler)()) {
    custom_timer_interrupt_handler = handler;
}
