#include "kernel/kernel.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/timer/timer.h"
#include "drivers/serial_port/serial_port.h"


void exception_handler(u32 interrupt, u32 error, char *message) {
    serial_log(LOG_ERROR, message);
}

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

void put_cursor(unsigned short pos) {
    out(0x3D4, 14);
    out(0x3D5, ((pos >> 8) & 0x00FF));
    out(0x3D4, 15);
    out(0x3D5, pos & 0x00FF);
}

_Noreturn void halt_loop() { while (1) { halt(); } }


char array[80];
int array_l = 0;
int cursor_x = 0;
int cursor_y = 0;
bool editMode = false;

void drawChar(int x, int y, char s) {
    char *framebuffer = (char *) 0xb8000;
    *(framebuffer + y * 80 * 2 + x * 2) = s;
}

char readChar(int x,int y) {
    char *framebuffer = (char *) 0xb8000;
    return *(framebuffer + y * 80 * 2 + x * 2);
}

void copyRow(int sourceRow, int destinationRow) {
    for(int x = 0; x < 80; x++) {
        char s=readChar(x, sourceRow);
        drawChar(x, destinationRow, s);
    }
}

void place_cursor() { put_cursor(cursor_y * 80 + cursor_x); }

void scroll(){
   for(int y = 0; y < 24; y++) {
    for(int x = 0; x < 80; x++) { drawChar(x, y, readChar(x, y + 1)); }
   }
   for(int x = 0; x < 80; x++) { drawChar(x, 24,' '); }
}

void print_char(char c) {   
    if(c == '\n') {
        if(cursor_y < 24) { cursor_y++; } else { scroll(); }
        cursor_x = 0;
    } else if(c == '\b') {
        if(cursor_x > 0) { cursor_x--; }
        drawChar(cursor_x, cursor_y, ' ');
    } else {
        drawChar(cursor_x, cursor_y, c);
        cursor_x++;
    }
    place_cursor();
}

void print(const char *message) {
    while (*message != '\0') {
        print_char(*message);
        message++;
    }
}

void split_string(char *input, char *token1, char *token2) {
    int i = 0, j = 0;

    while (input[i] != ' ' && input[i] != '\0') { token1[j++] = input[i++]; }
    token1[j] = '\0';

    while (input[i] == ' ') { i++; }

    j = 0;

    while (input[i] != '\0') { token2[j++] = input[i++]; }
    token2[j] = '\0';
}

int compare(char *str1, char *str2) {
    if(array_length(str1) != array_length(str2)) { return 0; }
    for(int i = 0; i < array_length(str1); i++) {
        if(str1[i] != str2[i])
        return 0;
    }
    return 1;
}

struct File {
    char name[20];
    char content[2000];
};

struct File files[10];
int sizeFile = 0;

void createFile(char *filename) {
    if (sizeFile < 10) {
        for (int i = 0; i < sizeFile; i++) {
            if (compare(files[i].name, filename) == 1) {
                print("File is already exists!\n");
                return;
            }
        }

        for (int i = 0; i < array_length(filename); i++) {
            files[sizeFile].name[i] = filename[i];
        }
        files[sizeFile].name[array_length(filename)] = '\0';
        sizeFile++;
        print("File is created!\n");

    } else { print("Maximum number of files reached!\n"); }
}

int cFile = 0;

void editFile(char *filename) {
    for (int i = 0; i < sizeFile; i++) {

        if (compare(files[i].name, filename) == 1) {
            cFile=i;
            print("Edit file: ");
            print(filename);
            print("\n");
            print(files[i].content);
            return;
        }
    }
    print("File not found!\n");
}

void readFile(char *filename) {
    for (int i = 0; i < sizeFile; i++) {
        if (compare(files[i].name, filename) == 1) {
            print("Content of file ");
            print(filename);
            print(":\n");

            char *content = files[i].content;
            print(content);
            print_char('\n');
            print_char('\n');
            return;
        }
    }
    print("File not found!\n");
}

void deleteFile(char *filename) {
    int fileIndex = -1;
    for (int i = 0; i < sizeFile; i++) {
        if (compare(files[i].name, filename) == 1) {
            fileIndex = i;
            break;
        }
    }

    if (fileIndex != -1) {
        for (int i = fileIndex; i < sizeFile - 1; i++) {
            for (int j = 0; j < array_length(files[i + 1].name); j++) {
                files[i].name[j] = files[i + 1].name[j];
            }
            for (int j = 0; j < array_length(files[i + 1].content); j++) {
                files[i].content[j] = files[i + 1].content[j];
            }
            files[i].name[array_length(files[i + 1].name)] = '\0';
            files[i].content[array_length(files[i + 1].content)] = '\0';
        }
        sizeFile--;
        print("File is deleted!\n");
    } else { print("File not found!\n"); }
}

void listFiles() {
    print("All files\n");
    for(int i = 0; i < sizeFile; i++) {
        char *filenames = files[i].name;
        while (*filenames != '\0') {
            print_char(*filenames);
            filenames++;
        }
        print_char('\n');
    }
}

int array_length(char array[]) {
    int counter = 0;
    while (array[counter] != '\0') { counter++; }
    return counter;
}

void clear() {
    int x, y;
    for (y = 0; y < 25; y++) {
        for (x = 0; x < 80; x++) { print_char(' '); }
    }
    cursor_x = 0;
    cursor_y = 0;
}

int compare_command(char *command) {
    if(array_l != array_length(command)) { return 0; }
    for(int i = 0; i  <array_l; i++) {
        if(array[i] != command[i])
        return 0;
    }
    return 1;
}


void clear_command() {
    int x, y;
    for (y = 0; y < 25; y++) {  
        for (x = 0; x < 80; x++) { drawChar(x, y, ' '); }
    }
    cursor_x = 0;
    cursor_y = 0;
}

void help_command() {
    print("clear - clear a display\n");
    print("create (filename) - create file\n");
    print("edit (filename) - edit text in file\n");
    print("read (filename) - read text in file\n");
    print("delete (filename) - delete file\n");
    print("list - view all files\n");
    
    int prev_cursor_x = cursor_x;
    int prev_cursor_y = cursor_y;
    cursor_x = 0;  
    cursor_y++;
    print_char('\n');
}

bool screensaver_active = false;
int cursor_speed_counter = 0;
int cursor_speed = 1.5;
#define SCREENSAVER_CHAR '@'

void screensaver() {
    static int screensaver_x = 0;
    static int screensaver_y = 0;
    static int screensaver_dx = 1;
    static int screensaver_dy = 1;

    clear_command();

    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 80; j++) {
            if ((j >= screensaver_x && j < screensaver_x + 4) && (i == screensaver_y || i == screensaver_y + 5)) {
                drawChar(j, i, SCREENSAVER_CHAR);
            } else if ((j == screensaver_x || j == screensaver_x + 4) && (i >= screensaver_y && i < screensaver_y + 6)) {
                drawChar(j, i, SCREENSAVER_CHAR);
            }
        }
    }

    screensaver_x += screensaver_dx;
    screensaver_y += screensaver_dy;

    if (screensaver_x >= 76 || screensaver_x < 0) {
        screensaver_dx = -screensaver_dx;
        screensaver_x += screensaver_dx;
    }

    if (screensaver_y >= 20 || screensaver_y < 0) {
        screensaver_dy = -screensaver_dy;
        screensaver_y += screensaver_dy;
    }
    put_cursor(-1);
}

#define KEY_ESC 27

char buffer[2000];
int buffer_l = 0;
int file_cursor = 0;

void key_handler(struct keyboard_event event) {
    if(editMode) {
        if(event.key_character && event.type == EVENT_KEY_PRESSED) {
            char key_char = event.key_character; 

            if (event.key != KEY_ENTER && event.key != KEY_BACKSPACE) {
                buffer[buffer_l] = key_char;
                buffer_l++;
                buffer[buffer_l] = '\0';
                print_char(event.key_character);
                files[cFile].content[file_cursor]=event.key_character;
                file_cursor++;
            }

            if (event.key == KEY_BACKSPACE) {
                print_char('\b');
                if (buffer_l > 0) { buffer_l--; }
                files[cFile].content[file_cursor] = ' ';
                file_cursor--;
            }

            if (event.key == KEY_ENTER) {
                if (compare(buffer, "-")) {
                    int len = array_length(files[cFile].content);
                    files[cFile].content[len - 1] = '\0';
                    file_cursor = 0;
                    editMode = false;
                    print("\nFile updated!\n\n");
                } else {
                    for (int i = buffer_l; i < 80; i++) {
                        files[cFile].content[i] = ' ';
                        file_cursor++;
                    }
                    print_char('\n');
                    buffer_l = 0;
                }    
            }
        }
    } else if (event.key_character && event.type == EVENT_KEY_PRESSED) {
        char key_char = event.key_character; 

        if (event.key != KEY_ENTER && event.key != KEY_BACKSPACE) { 
            array[array_l] = key_char;
            array_l++;
            array[array_l] = '\0';
            print_char(event.key_character);
        }

        if (screensaver_active) {
            screensaver_active = false;
            clear_command();
            return;
        }

        if (event.key == KEY_BACKSPACE) {
            print_char('\b');
            if (array_l > 0) { array_l--; }
        }

        if (event.key == KEY_ENTER) {
            char command[80], argument[80];
            split_string(array, command, argument);

            if (compare(command, "clear")) { clear_command(); } 
            else if (compare(command, "help")) {
                print_char('\n');
                help_command();
            } else if (compare(command, "sleep")) {
                screensaver_active = !screensaver_active;
                if (screensaver_active) { screensaver(); } else { clear(); }

            } else if (compare(command, "create") && argument[0] != '\0') {
                print_char('\n');
                createFile(argument);
                editMode = false;
                print_char('\n');
                array_l = 0;
            } else if(compare(command, "delete")&& argument[0]!='\0') {
                print_char('\n');
                deleteFile(argument);
                array_l = 0;
            } else if(compare(command, "edit")&& argument[0]!='\0') {
                editMode = true;
                print_char('\n');
                editFile(argument);
                array_l = 0;
            } else if(compare(command, "read")&& argument[0]!='\0') {
                print_char('\n');
                readFile(argument);
                array_l = 0;
            } else if (compare(command, "list")) {
                print_char('\n');
                listFiles();
            } else {
                print_char('\n');
                print("Command not found\n");
                array_l = 0;
            }
        }
    }
}

void timer_tick_handler() {
    if (screensaver_active) {
        cursor_speed_counter = (cursor_speed_counter + 1) % cursor_speed;
        if (cursor_speed_counter == 0) { screensaver(); }
    }
}

void kernel_entry() {
    init_kernel();
    keyboard_set_handler(key_handler);
    timer_set_handler(timer_tick_handler);
    clear();
    halt_loop();
}