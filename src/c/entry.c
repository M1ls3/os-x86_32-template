#include "kernel/kernel.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/timer/timer.h"
#include "drivers/serial_port/serial_port.h"

#define rows 25
#define column 80
#define startSymbl '$'

void exception_handler(u32 interrupt, u32 error, char *message)
{
    serial_log(LOG_ERROR, message);
}

void init_kernel()
{
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

/**
 * Puts cursors in a given position. For example, position = 20 would place it in
 * the first line 20th column, position = 80 will place in the first column of the second line.
 */
void put_cursor(unsigned short pos)
{
    out(0x3D4, 14);
    out(0x3D5, ((pos >> 8) & 0x00FF));
    out(0x3D4, 15);
    out(0x3D5, pos & 0x00FF);
}

/**
 * In order to avoid execution of arbitrary instructions by CPU we halt it.
 * Halt "pauses" CPU and puts it in low power mode until next interrupt occurs.
 */
_Noreturn void halt_loop()
{
    while (1)
    {
        halt();
    }
}

char *commands[] = {"clear", "help", "list", "create", "edit", "delete", "read", "sleep"};
char *framebuffer = (char *)0xb8000;
char array[80];
int array_l = 0;
int color = 0xa;
int lenCommand = 0;
bool editMode = false;

int x = 2;
int y = 0;

#define KEY_ESC 27

char buffer[2000];
int buffer_l = 0;
int file_cursor = 0;
int iFile = 0;

bool screensaver_active = false;
int cursor_speed_counter = 0;
int cursor_speed = 1.8;
#define SCREENSAVER_CHAR '#'

void key_handler(struct keyboard_event event)
{
    if (editMode)
    {
        fileContentEdit(event);
        scroll();
    }
    else if (event.key_character && event.type == EVENT_KEY_PRESSED)
    {
        if (event.key == KEY_ENTER)
        {
            commandHendle();
            array[0] = '\0';
            array_l = 0; 
        }

        else if (event.key == KEY_BACKSPACE)
        {
            if (x > 2 && array_l > 0)
            {
                array_l--;
                array[array_l] = '\0';
                --x;
                setSymbol(x, y, ' ', color);
                put_cursor(getCursor(x, y) - 1);
            }
        }
        else if (event.key == KEY_TAB)
        {
            screensaver_active = false;
            clear();
        }
        else
        {
            if (x < column - 1)
            {
                array[array_l] = event.key_character;
                array_l++;
                array[array_l] = '\0';
                setSymbol(x, y, event.key_character, color);
                put_cursor(getCursor(x, y));
                x++;
            }
        }
    }
}

void timer_tick_handler()
{
    if (screensaver_active)
    {
        cursor_speed_counter = (cursor_speed_counter + 1) % cursor_speed;
        if (cursor_speed_counter == 0)
        {
            screensaver();
        }
    }
}

void commandHendle()
{
    char command[80], argument[80]; // \0
    split(array, command, argument); // \0
    if (compare(command, commands[0])) // clear
    {
        clear();
    }
    else if (compare(command, commands[1])) // help
    {
        display_help();
    }
    else if (compare(command, commands[2])) // list
    {
        listFiles();
    }
    else if (compare(command, commands[3])) // create
    {
        createFile(argument);
    }
    else if (compare(command, commands[4])) // edit
    {
        editFile(argument);
    }
    else if (compare(command, commands[5])) // delete
    {
        deleteFile(argument);
    }
    else if (compare(command, commands[6])) // read
    {
        readFile(argument);
    }
    else if (compare(command, commands[7])) // sleep
    {
        screensaver_active = !screensaver_active;
        if (screensaver_active)
        {
            screensaver();
        }
        else
        {
            clear();
        }
    }
    else
    {
        print("Command not exist", color);
        newLine();
        scroll();
    }
}

void newLine()
{
    y++;
    x = 0;
    setSymbol(x, y, startSymbl, color);
    x = 1;
    put_cursor(getCursor(x, y));
    x = 2;
    scroll();
}

void clear()
{
    x = 0;
    y = 0;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < column; j++)
        {
            setSymbol(j, i, ' ', color);
        }
    }
    setSymbol(x, y, startSymbl, color);
    put_cursor(2);
    x = 2;
    y = 0;
}

void clearSleep()
{
    x = 0;
    y = 0;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < column; j++)
        {
            setSymbol(j, i, ' ', color);
        }
    }
    put_cursor(-1);
    x = 0;
    y = 0;
}

void display_help()
{
    for (int i = 0; i < 8; i++)
    {
        char *command = commands[i];
        print(command, color);
        scroll();
    }
    newLine();
    scroll();
}

void print(char *msg, char color)
{
    y++;
    x = 0;
    while (*msg != '\0')
    {
        setSymbol(x, y, *msg, color);
        x++;
        if (x >= 80)
        {
            x = 0;
            y++;
        }
        msg++;
        if (editMode)
            scrollFile();
    }
    scroll();
}

void scroll()
{
    while (y > rows - 2)
    {
        int l1 = 0;
        int l2 = 1;
        x = 0;
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < column; j++)
            {
                setSymbol(x, l1, getSymbol(x, l2, 0), getSymbol(x, l2, 1));
                x++;
            }
            x = 0;
            l1++;
            l2++;
        }
        y--;
        x = 2;
    }
}

void scrollFile()
{
    while (y > rows - 2)
    {
        int l1 = 0;
        int l2 = 1;
        x = 0;
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < column; j++)
            {
                setSymbol(x, l1, getSymbol(x, l2, 0), getSymbol(x, l2, 1));
                x++;
            }
            x = 0;
            l1++;
            l2++;
        }
        y--;
        x = 0;
    }
}

void setSymbol(int xPos, int yPos, char symbl, char color)
{
    int newPosition = (xPos * 2) + (column * 2 * yPos);

    framebuffer[newPosition] = symbl;
    framebuffer[newPosition + 1] = color;
}

int getSymbol(int xPos, int yPos, int color)
{ // if you want to get a color, put 1
    if (color > 1)
    {
        color = 0;
    }
    int newPosition = (xPos * 2) + (column * 2 * yPos) + color;

    return framebuffer[newPosition];
}

int getCursor(int xPos, int yPos)
{
    if (yPos < 25)
    {
        return xPos + (column * yPos) + 1;
    }
    else
    {
        return xPos + (column * (yPos - 1)) + 1;
    }
}

int array_length(char array[])
{
    int counter = 0;
    while (array[counter] != '\0')
    {
        counter++;
    }
    return counter;
}

int compare(char *str1, char *str2)
{
    if (array_length(str1) != array_length(str2))
    {
        return 0;
    }
    for (int i = 0; i < array_length(str1); i++)
    {
        if (str1[i] != str2[i])
            return 0;
    }
    return 1;
}

void split(char *input, char *token1, char *token2)
{
    int i = 0, j = 0;

    while (input[i] != ' ' && input[i] != '\0')
    {
        token1[j++] = input[i++];
    }
    token1[j] = '\0';

    while (input[i] == ' ')
    {
        i++;
    }

    j = 0;

    while (input[i] != '\0')
    {
        token2[j++] = input[i++];
    }
    token2[j] = '\0';
}

struct File
{
    char fileName[20];
    char content[80 * 25];
    int contentLength;
};

struct File files[10];
int fileAmount = 0;

int IsFrog(char *filename) // frog (oprional)
{
    int length = array_length(filename);
    if (length > 3)
    {
        for (int i = 0; i < length; i++)
        {
            if (filename[i] == 'f' && filename != "\0")
                if (filename[i + 1] == 'r' && filename != "\0")
                    if (filename[i + 2] == 'o' && filename != "\0")
                        if (filename[i + 3] == 'g' && filename != "\0")
                            return 1;
        }
    }
    return 0;
}

void createFile(char *filename)
{
    if (fileAmount < 10)
    {
        if (compare(filename, "\0") || compare(filename, " ") || IsFrog(filename)) // IsFrog (Optional)
        {
            print("Error! Incorrect file name!", color);
            newLine();
            scroll();
            return;
        }
        for (int i = 0; i < fileAmount; i++)
        {
            if (compare(files[i].fileName, filename))
            {
                print("Error! File is already exists!", color);
                newLine();
                scroll();
                return;
            }
        }

        for (int i = 0; i < array_length(filename); i++)
        {
            files[fileAmount].fileName[i] = filename[i];
        }
        files[fileAmount].fileName[array_length(filename)] = '\0';
        fileAmount++;
        print("Success! File is created!", color);
        scroll();
    }
    else
    {
        print("Error! Maximum number of files reached!", color);
        scroll();
    }
    newLine();
    scroll();
}

void deleteFile(char *filename)
{
    int fileIndex = -1;
    for (int i = 0; i < fileAmount; i++)
    {
        if (compare(files[i].fileName, filename))
        {
            fileIndex = i;
            break;
        }
    }

    if (fileIndex != -1)
    {
        for (int i = fileIndex; i < fileAmount - 1; i++)
        {
            for (int j = 0; j < array_length(files[i + 1].fileName); j++)
            {
                files[i].fileName[j] = files[i + 1].fileName[j];
            }
            for (int j = 0; j < array_length(files[i + 1].content); j++)
            {
                files[i].content[j] = files[i + 1].content[j];
            }
            files[i].fileName[array_length(files[i + 1].fileName)] = '\0';
            files[i].content[array_length(files[i + 1].content)] = '\0';
        }
        fileAmount--;
        print("File is deleted!", color);
        newLine();
        scroll();
    }
    else
    {
        print("File not found!", color);
        newLine();
        scroll();
    }
    scroll();
}

void listFiles()
{
    print("All files", color);
    for (int i = 0; i < fileAmount; i++)
    {
        print(files[i].fileName, color);
        scroll();
    }
    newLine();
    scroll();
}

void editFile(char *filename)
{
    for (int i = 0; i < fileAmount; i++)
    {
        if (compare(files[i].fileName, filename))
        {
            editMode = true;
            iFile = i;
            print("Edit file:", color);
            scrollFile();
            print(filename, color);
            scrollFile();
            print(files[i].content, color);
            put_cursor(getCursor(x, y) - 1);
            scrollFile();
            return;
        }
    }
    print("File not found!", color);
    newLine();
    scroll();
}

void fileContentEdit(struct keyboard_event event)
{
    if (event.key_character && event.type == EVENT_KEY_PRESSED)
    {
        char key_char = event.key_character;
        if (event.key == KEY_BACKSPACE)
        {
            if (files[iFile].contentLength > 0)
            {
                x--;
                setSymbol(x, y, ' ', color);
                put_cursor(getCursor(x, y) - 1);
                files[iFile].content[files[iFile].contentLength] = ' ';
                files[iFile].contentLength--;
            }
        }

        else if (event.key == KEY_ENTER)
        {
            if (files[iFile].contentLength < 80 * 25)
            {
                for (int i = x; i < column; i++)
                {
                    x++;
                  
                    setSymbol(x, y, ' ', color);
                    files[iFile].content[files[iFile].contentLength] = ' ';
                    files[iFile].contentLength++;
                    scrollFile();
                    
                }
                y++;
                x = 0;
                put_cursor(getCursor(x, y));
                scrollFile();
            }
        }
        else if (event.key == KEY_TAB)
        {
            files[iFile].content[files[iFile].contentLength - 1] = '\0';
            editMode = false;
            put_cursor(getCursor(x, y));
            print("File updated!", color);
            newLine();
            scroll();
            return;
        }
        else
        {
            if (files[iFile].contentLength < 80 * 25)
            {
                int copyX = x;
                buffer[copyX] = key_char;
                setSymbol(x, y, event.key_character, color);
                put_cursor(getCursor(x, y));
                files[iFile].content[files[iFile].contentLength] = event.key_character;
                files[iFile].contentLength++;
                x++;
                copyX++;
                if (x >= 80)
                {
                    x = 0;
                    y++;
                }
                buffer[copyX] = '\0';
                scrollFile();
            }
        }
    }
}

void readFile(char *filename)
{
    for (int i = 0; i < fileAmount; i++)
    {
        if (compare(files[i].fileName, filename))
        {
            print(filename, color);
            scroll();
            char *content = files[i].content;
            print(content, color);
            newLine();
            scroll();
            return;
        }
    }
    print("File not found!", color);
    newLine();
    scroll();
}

void screensaver()
{
    static int screensaver_x = 0;
    static int screensaver_y = 0;
    static int screensaver_dx = 1;
    static int screensaver_dy = 1;

    clearSleep();

    for (int i = 0; i < 25; i++)
    {
        for (int j = 0; j < 80; j++)
        {
            if ((j >= screensaver_x && j < screensaver_x + 4) && (i == screensaver_y || i == screensaver_y + 5))
            {
                setSymbol(j, i, SCREENSAVER_CHAR, color);
            }
            else if ((j == screensaver_x || j == screensaver_x + 4) && (i >= screensaver_y && i < screensaver_y + 6))
            {
                setSymbol(j, i, SCREENSAVER_CHAR, color);
            }
        }
    }

    screensaver_x += screensaver_dx;
    screensaver_y += screensaver_dy;

    if (screensaver_x >= 76 || screensaver_x < 0)
    {
        screensaver_dx = -screensaver_dx;
        screensaver_x += screensaver_dx;
    }

    if (screensaver_y >= 20 || screensaver_y < 0)
    {
        screensaver_dy = -screensaver_dy;
        screensaver_y += screensaver_dy;
    }
    put_cursor(-1);
}

/**
 * This is where the bootloader transfers control to.
 */
void kernel_entry()
{
    init_kernel();
    keyboard_set_handler(key_handler);
    timer_set_handler(timer_tick_handler);

    clear();

    halt_loop();
}