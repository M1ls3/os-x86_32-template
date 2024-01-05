#include "serial_port.h"
#include "../../kernel/kernel.h"

#define SERIAL_COM1_BASE                0x3F8
#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

void configure_default_serial_port() {
    out(SERIAL_LINE_COMMAND_PORT(SERIAL_COM1_BASE), 0x80);  // Enable DLAB (set baud rate divisor)
    out(SERIAL_DATA_PORT(SERIAL_COM1_BASE), 0x03);          // Set divisor to 3 (lo byte) 38400 baud
    out(SERIAL_LINE_COMMAND_PORT(SERIAL_COM1_BASE), 0x03);  // 8 bits, no parity, one stop bit
    out(SERIAL_FIFO_COMMAND_PORT(SERIAL_COM1_BASE), 0xC7);  // Enable FIFO, clear them, with 14-byte threshold
    out(SERIAL_MODEM_COMMAND_PORT(SERIAL_COM1_BASE), 0x0B); // IRQs enabled, RTS/DSR set
}

int in_queue_state(u32 com) {
    return in(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

void serial_print_char(char c) {
    while (in_queue_state(SERIAL_COM1_BASE) == 0);
    out(SERIAL_DATA_PORT(SERIAL_COM1_BASE), c);
}

void serial_print(const char *str) {
    while (*str) {
        serial_print_char(*str++);
    }
}

void serial_log(enum log_level level, const char *message) {
    serial_print_char('[');
    switch (level) {
        case LOG_INFO:
            serial_print("INFO");
            break;
        case LOG_WARNING:
            serial_print("WARNING");
            break;
        case LOG_ERROR:
            serial_print("ERROR");
            break;
        default:
            serial_print("UNKNOWN");
            break;
    }
    serial_print_char(']');
    serial_print_char(' ');
    serial_print(message);
    serial_print("\r\n");
}
