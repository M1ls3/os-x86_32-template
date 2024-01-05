#include "../../kernel/kernel.h"
#include "keyboard.h"

#define KEYBOARD_DATA_PORT 0x60
#define SCANCODES_KNOWN 89

// https://wiki.osdev.org/Keyboard#Scan_Code_Set_1
u8 scancode_to_key[SCANCODES_KNOWN] = {
    0,
    KEY_ESC,
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
    0,
    0,
    0,
    KEY_F11,
    KEY_F12
};

char key_to_character[SCANCODES_KNOWN];

void (*custom_key_handler)(struct keyboard_event event) = 0;

/* Handles the keyboard interrupt */
void keyboard_handler(__attribute__((unused)) u32 interrupt) {
    const u8 scancode = in(KEYBOARD_DATA_PORT);
    if (custom_key_handler != 0) {
        // 0x80 bit says it's released, otherwise it's pressed
        enum key_event_type event_type;
        if (scancode & 0x80) {
            event_type = EVENT_KEY_RELEASED;
        } else {
            event_type = EVENT_KEY_PRESSED;
        }

        // cut the event bit (allows to use a single table for keys)
        const u8 bare_scancode = scancode & (0x80 - 1);
        if (bare_scancode < SCANCODES_KNOWN) {
            struct keyboard_event event;
            event.type = event_type;
            event.key  = scancode_to_key[bare_scancode];
            event.key_character = key_to_character[event.key];
            custom_key_handler(event);
        }
    }
}

void map_keys_to_characters() {
    key_to_character[KEY_1]                       = '1';
    key_to_character[KEY_2]                       = '2';
    key_to_character[KEY_3]                       = '3';
    key_to_character[KEY_4]                       = '4';
    key_to_character[KEY_5]                       = '5';
    key_to_character[KEY_6]                       = '6';
    key_to_character[KEY_7]                       = '7';
    key_to_character[KEY_8]                       = '8';
    key_to_character[KEY_9]                       = '9';
    key_to_character[KEY_0]                       = '0';
    key_to_character[KEY_MINUS]                   = '-';
    key_to_character[KEY_EQUALS]                  = '=';
    key_to_character[KEY_BACKSPACE]               = '\b';
    key_to_character[KEY_TAB]                     = '\t';
    key_to_character[KEY_Q]                       = 'q';
    key_to_character[KEY_W]                       = 'w';
    key_to_character[KEY_E]                       = 'e';
    key_to_character[KEY_R]                       = 'r';
    key_to_character[KEY_T]                       = 't';
    key_to_character[KEY_Y]                       = 'y';
    key_to_character[KEY_U]                       = 'u';
    key_to_character[KEY_I]                       = 'i';
    key_to_character[KEY_O]                       = 'o';
    key_to_character[KEY_P]                       = 'p';
    key_to_character[KEY_LEFT_RECTANGLE_BRACKET]  = '[';
    key_to_character[KEY_RIGHT_RECTANGLE_BRACKET] = ']';
    key_to_character[KEY_ENTER]                   = '\n';
    key_to_character[KEY_A]                       = 'a';
    key_to_character[KEY_S]                       = 's';
    key_to_character[KEY_D]                       = 'd';
    key_to_character[KEY_F]                       = 'f';
    key_to_character[KEY_G]                       = 'g';
    key_to_character[KEY_H]                       = 'h';
    key_to_character[KEY_J]                       = 'j';
    key_to_character[KEY_K]                       = 'k';
    key_to_character[KEY_L]                       = 'l';
    key_to_character[KEY_SEMICOLON]               = ';';
    key_to_character[KEY_SINGLE_QUOTE]            = '\'';
    key_to_character[KEY_BACK_TICK]               = '`';
    key_to_character[KEY_BACK_SLASH]              = '\\';
    key_to_character[KEY_Z]                       = 'z';
    key_to_character[KEY_X]                       = 'x';
    key_to_character[KEY_C]                       = 'c';
    key_to_character[KEY_V]                       = 'v';
    key_to_character[KEY_B]                       = 'b';
    key_to_character[KEY_N]                       = 'n';
    key_to_character[KEY_M]                       = 'm';
    key_to_character[KEY_COMMA]                   = ',';
    key_to_character[KEY_DOT]                     = '.';
    key_to_character[KEY_SLASH]                   = '/';
    key_to_character[KEY_KEYPAD_STAR_PRESSED]     = '*';
    key_to_character[KEY_SPACE]                   = ' ';
    key_to_character[KEY_KEYPAD_7]                = '7';
    key_to_character[KEY_KEYPAD_8]                = '8';
    key_to_character[KEY_KEYPAD_9]                = '9';
    key_to_character[KEY_KEYPAD_MINUS]            = '-';
    key_to_character[KEY_KEYPAD_4]                = '4';
    key_to_character[KEY_KEYPAD_5]                = '5';
    key_to_character[KEY_KEYPAD_6]                = '6';
    key_to_character[KEY_KEYPAD_PLUS]             = '+';
    key_to_character[KEY_KEYPAD_1]                = '1';
    key_to_character[KEY_KEYPAD_2]                = '2';
    key_to_character[KEY_KEYPAD_3]                = '3';
    key_to_character[KEY_KEYPAD_0]                = '0';
    key_to_character[KEY_KEYPAD_DOT]              = '.';
}

void register_keyboard_interrupt_handler() {
    map_keys_to_characters();
    set_interrupt_handler(INTERRUPT_KEYBOARD, keyboard_handler);
}

void keyboard_set_handler(void (*handler)(struct keyboard_event event)) {
    custom_key_handler = handler;
}
