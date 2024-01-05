#ifndef SERIAL_H
#define SERIAL_H

enum log_level {
    LOG_INFO = 1,
    LOG_ERROR,
    LOG_WARNING
};

/**
 * Configures default (COM1) serial port.
 * This has to be called before the serial_print cna be used.
 */
extern void configure_default_serial_port();

/**
 * Prints the given message to the default serial port.
 */
extern void serial_print(const char *str);

/**
 * Prints a message to serial following the logging format.
 * Each message is followed by \r\n.
 */
extern void serial_log(enum log_level level, const char *message);

#endif
