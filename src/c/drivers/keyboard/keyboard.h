#ifndef KEYBOARD_H
#define KEYBOARD_H

enum key {
    KEY_ESC = 1,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_0,
    KEY_MINUS,
    KEY_EQUALS,
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_Q,
    KEY_W,
    KEY_E,
    KEY_R,
    KEY_T,
    KEY_Y,
    KEY_U,
    KEY_I,
    KEY_O,
    KEY_P,
    KEY_LEFT_RECTANGLE_BRACKET,
    KEY_RIGHT_RECTANGLE_BRACKET,
    KEY_ENTER,
    KEY_LEFT_CONTROL,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_SEMICOLON,
    KEY_SINGLE_QUOTE,
    KEY_BACK_TICK,
    KEY_LEFT_SHIFT,
    KEY_BACK_SLASH,
    KEY_Z,
    KEY_X,
    KEY_C,
    KEY_V,
    KEY_B,
    KEY_N,
    KEY_M,
    KEY_COMMA,
    KEY_DOT,
    KEY_SLASH,
    KEY_RIGHT_SHIFT,
    KEY_KEYPAD_STAR_PRESSED,
    KEY_LEFT_ALT,
    KEY_SPACE,
    KEY_CAPSLOCK,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_NUM_LOCK,
    KEY_SCROLL_LOCK,
    KEY_KEYPAD_7,
    KEY_KEYPAD_8,
    KEY_KEYPAD_9,
    KEY_KEYPAD_MINUS,
    KEY_KEYPAD_4,
    KEY_KEYPAD_5,
    KEY_KEYPAD_6,
    KEY_KEYPAD_PLUS,
    KEY_KEYPAD_1,
    KEY_KEYPAD_2,
    KEY_KEYPAD_3,
    KEY_KEYPAD_0,
    KEY_KEYPAD_DOT,
    KEY_F11,
    KEY_F12
};

enum key_event_type {
    EVENT_KEY_PRESSED = 1,
    EVENT_KEY_RELEASED
};

struct keyboard_event {
    enum key            key;
    enum key_event_type type;
    char                key_character;
};

/**
 * Registers keyboard interrupt handler that is capable to
 * translate scancodes to keyboard_event values. Use keyboard_set_handler
 * function to consume those events.
 */
extern void register_keyboard_interrupt_handler();

/**
 * Sets the given function as the handler of the keyboard events. Calling
 * this function twice will override the previously set function value.
 */
extern void keyboard_set_handler(void (*handler)(struct keyboard_event event));

#endif
