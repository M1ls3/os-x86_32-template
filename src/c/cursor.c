#include "kernel/kernel.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/timer/timer.h"
#include "drivers/serial_port/serial_port.h"


#define rows 25
#define column 80
#define startLine '$'
void exception_handler(u32 interrupt, u32 error, char *message) {
    serial_log(LOG_ERROR, message);
}

void putSymbol(int x, int y, char symbl, char color);

void init_kernel() {
    init_gdt();
    init_idt();
    init_exception_handlers();
    init_interrupt_handlers();
    register_timer_interrupt_handler();
    register_keyboard_interrupt_handler();
    configure_default_serial_port();
    set_exception_handler(exception_handler);
    enable_interrupts();
}



char *commands[] = { "clear", "help" };
char *framebuffer = (char *)0xb8000;
int lenCommand = 0;

int x = 0;
int y = 0;


//y = current position by y
//x = current position by x
//lenCommand = length of typing command
//method putSymbol(x,y,symbol,color)
//method getNewCursor calculate new cursor position relative to x, and y coordinate and add 1, cuz cursor must be a step over of symbol
//method printMessage, take arguments message in "" and color of symbols
//method getSymbol, return a symbol in position relative to x and y arguments

void clear() {
    
    x = 0;
    y = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < column; j++) {
            putSymbol(x, y, ' ', 0x7);
            x++;
        }
        x = 0;
        y+=1;
    
    }
    putStartLine();

    x = 2;
    y = 0;

    put_cursorr(getNewCursor(x,y)-1);
}

void putStartLine() {
    putSymbol(0, 0, startLine, 0x2);
}

void scroll_screen() {
    
    while (y >= 24) {
        overflow();

    }
}

void overflow() {

    int l1 = 0;
    int l2 = 1;
    x = 0;
    for (int i = 0; i < column; i++) {
        for (int j = 0; j < rows; j++) {

            putSymbol(x, l1, getSymbol(x,l2,0), getSymbol(x,l2,1));
            x++;
        }
        x = 0;
        l1++;
        l2++;
    }
    y--;
    x = 2;

}

void put_cursorr(unsigned short pos) {
    out(0x3D4, 14);
    out(0x3D5, ((pos >> 8) & 0x00FF));
    out(0x3D4, 15);
    out(0x3D5, pos & 0x00FF);
}

void halt_loop() {
    while (1) { halt(); }
}

void key_handler(struct keyboard_event event) {

    if (event.key_character && event.type == EVENT_KEY_PRESSED) {
        // Enter
        
        if (event.key == KEY_ENTER) {
            
            int commandNumber = -1;
            int tmp = x;
            
            for (int i = 0; i < 2; i++) {
                char *command = commands[i];
                while (*command != '\0' || getSymbol(x-lenCommand,y,0) != ' ') {
                    if (*command == getSymbol(x-lenCommand,y,0)) {
                        commandNumber = i;
                    } else {
                        commandNumber = -1;
                    }

                    x++;
                    command++;
                }
                x = tmp;
                if (commandNumber >= 0) {
                    break;
                }
            }
            

            lenCommand = 0;
            commandHandle(commandNumber);
        } else if (event.key == KEY_BACKSPACE) {
            if (x > 2) {
                --x;
                putSymbol(x,y,' ', 0x7);
                put_cursorr(getNewCursor(x,y)-1);
                lenCommand--;
            }
           
        } else {
            if (x < column-1){
                lenCommand++;
                putSymbol(x, y, event.key_character, 0x7);
                put_cursorr(getNewCursor(x,y));
                x += 1;
            }
        }
    }
}

void commandHandle(int position) {

    switch (position) {
        case 0:
            clear();
            break;
        case 1:
            printHelp();
            scroll_screen();
            newLine();
            break;
        default:
            printMessage("Wrong command", 0x4);
            newLine();
            break;
    }
}

void printHelp() {
    y++; // move next line
    x = 0;
    for (int i = 0; i < 2; i++) {
        char *command = commands[i];

        while (*command != '\0') {
            putSymbol(x,y, *command, 0x7);
            command++;
            x++;
        }
        x = 0;
        y++;
        
    }
    y -= 1;

}

void printMessage(char *msg, char color) {
    y++;
    x = 0;
    while (*msg != '\0') {
        putSymbol(x,y, *msg, color);
        x++;
        msg++;
    }
    scroll_screen();

}

void newLine() {
    y++;
    x = 0;
    putSymbol(x, y, startLine, 0x2);
    x = 1;
    put_cursorr(getNewCursor(x,y));
    x = 2;
}

void putSymbol(int xPos, int yPos, char symbl, char color) {

    int newPosition = (xPos*2) + (column * 2 * yPos);

    framebuffer[newPosition] = symbl;
    framebuffer[newPosition + 1] = color;
}

int getSymbol(int xPos, int yPos, int color) { // if you want to get a color, put 1
    if (color > 1) {
        color = 0;
    }
    int newPosition = (xPos*2) + (column * 2 * yPos) + color;

    return framebuffer[newPosition];
}

int getNewCursor(int xPos, int yPos) {
    return xPos+(column * yPos) + 1;
}


void timer_tick_handler() {
    // do something when timer ticks
}

/**
 * This is where the bootloader transfers control to.
 */
void kernel_entry() {
    init_kernel();
    keyboard_set_handler(key_handler);
    timer_set_handler(timer_tick_handler);
    clear();

    

    halt_loop();
}
