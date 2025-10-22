#include "../../Lib/include/terminal.h"
#include "../../Lib/include/keyboard.h"
#include "../../Lib/include/graphics.h"

#define MAX_COMMAND_LENGTH 256

// Command buffer
static char command_buffer[MAX_COMMAND_LENGTH];
static int cmd_index = 0;

// String utilities (same as before)
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

void strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

int starts_with(const char* str, const char* prefix) {
    while (*prefix) {
        if (*str != *prefix) return 0;
        str++;
        prefix++;
    }
    return 1;
}

void itoa(int num, char* str) {
    int i = 0;
    int is_negative = 0;
    
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }
    
    while (num != 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }
    
    if (is_negative) {
        str[i++] = '-';
    }
    
    str[i] = '\0';
    
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// Print prompt
void shell_prompt(terminal_t* term) {
    color_t green = {0, 255, 0, 255};
    color_t white = COLOR_WHITE;
    color_t cyan = {255, 255, 0, 255};
    color_t transparent = {0, 0, 0, 180};
    
    terminal_set_color(term, green, transparent);
    terminal_print(term, "root@seppuku");
    terminal_set_color(term, white, transparent);
    terminal_print(term, ":");
    terminal_set_color(term, cyan, transparent);
    terminal_print(term, "~");
    terminal_set_color(term, white, transparent);
    terminal_print(term, "$ ");
    terminal_render(term);
}

// Execute command
void shell_execute(terminal_t* term, const char* cmd) {
    color_t white = COLOR_WHITE;
    color_t cyan = {255, 255, 0, 255};
    color_t red = {0, 0, 255, 255};
    color_t green = {0, 255, 0, 255};
    color_t yellow = {0, 255, 255, 255};
    color_t gray = {128, 128, 128, 255};
    color_t transparent = {0, 0, 0, 180};
    
    if (cmd[0] == '\0') {
        return;
    }
    
    terminal_set_color(term, white, transparent);
    
    // HELP
    if (strcmp(cmd, "help") == 0) {
        terminal_set_color(term, cyan, transparent);
        terminal_println(term, "Available commands:");
        terminal_set_color(term, white, transparent);
        terminal_println(term, "  help    - Show this help");
        terminal_println(term, "  clear   - Clear screen");
        terminal_println(term, "  about   - About this OS");
        terminal_println(term, "  echo    - Echo text");
        terminal_println(term, "  test    - Graphics test");
        terminal_println(term, "  reboot  - Reboot system");
        terminal_render(term);
        return;
    }
    
    // CLEAR
    if (strcmp(cmd, "clear") == 0) {
        terminal_clear(term);
        graphics_load_wallpaper();
        terminal_render(term);
        return;
    }
    
    // ABOUT
    if (strcmp(cmd, "about") == 0) {
        terminal_set_color(term, red, transparent);
        terminal_println(term, "SEPPUKU OS v2.0 GRAPHICAL");
        terminal_set_color(term, white, transparent);
        terminal_println(term, "");
        terminal_println(term, "Features:");
        terminal_println(term, "  - VESA graphics 1024x768x32");
        terminal_println(term, "  - Transparent terminal");
        terminal_println(term, "  - Gradient wallpaper");
        terminal_println(term, "  - Software font rendering");
        terminal_println(term, "  - Alpha blending");
        terminal_render(term);
        return;
    }
    
    // ECHO
    if (starts_with(cmd, "echo ")) {
        terminal_set_color(term, yellow, transparent);
        terminal_println(term, cmd + 5);
        terminal_set_color(term, white, transparent);
        terminal_render(term);
        return;
    }
    
    // TEST (graphics test)
    if (strcmp(cmd, "test") == 0) {
        terminal_println(term, "Running graphics test...");
        terminal_render(term);
        
        // Draw some shapes on the wallpaper
        graphics_load_wallpaper();
        
        // Draw rectangles
        graphics_fill_rect(50, 50, 100, 100, (color_t){0, 0, 255, 200});
        graphics_fill_rect(170, 50, 100, 100, (color_t){0, 255, 0, 200});
        graphics_fill_rect(290, 50, 100, 100, (color_t){255, 0, 0, 200});
        
        // Draw some lines
        graphics_draw_line(50, 200, 400, 350, COLOR_YELLOW);
        graphics_draw_line(400, 200, 50, 350, COLOR_CYAN);
        
        // Small delay
        for (volatile int i = 0; i < 30000000; i++);
        
        // Redraw wallpaper and terminal
        graphics_load_wallpaper();
        terminal_render(term);
        
        terminal_set_color(term, green, transparent);
        terminal_println(term, "Test complete!");
        terminal_set_color(term, white, transparent);
        terminal_render(term);
        return;
    }
    
    // REBOOT
    if (strcmp(cmd, "reboot") == 0) {
        terminal_set_color(term, yellow, transparent);
        terminal_println(term, "Rebooting...");
        terminal_render(term);
        
        for (volatile int i = 0; i < 10000000; i++);
        
        unsigned char temp;
        __asm__ __volatile__("inb %1, %0" : "=a"(temp) : "Nd"((unsigned short)0x64));
        while (temp & 0x02) {
            __asm__ __volatile__("inb %1, %0" : "=a"(temp) : "Nd"((unsigned short)0x64));
        }
        __asm__ __volatile__("outb %0, %1" : : "a"((unsigned char)0xFE), "Nd"((unsigned short)0x64));
        __asm__ __volatile__("hlt");
        return;
    }
    
    // Unknown command
    terminal_set_color(term, red, transparent);
    terminal_print(term, "Command not found: ");
    terminal_set_color(term, white, transparent);
    terminal_println(term, cmd);
    terminal_set_color(term, gray, transparent);
    terminal_println(term, "Type 'help' for commands");
    terminal_set_color(term, white, transparent);
    terminal_render(term);
}

// Initialize shell
void shell_init_graphical(terminal_t* term) {
    cmd_index = 0;
    
    color_t cyan = {255, 255, 0, 255};
    color_t gray = {128, 128, 128, 255};
    color_t white = COLOR_WHITE;
    color_t transparent = {0, 0, 0, 180};
    
    terminal_set_color(term, cyan, transparent);
    terminal_println(term, "");
    terminal_println(term, "Welcome to SEPPUKU OS Graphical Shell!");
    terminal_set_color(term, gray, transparent);
    terminal_println(term, "Type 'help' for available commands.");
    terminal_set_color(term, white, transparent);
    terminal_println(term, "");
    terminal_render(term);
    
    shell_prompt(term);
}

// Handle key input
void shell_handle_key_graphical(terminal_t* term, char c) {
    if (c == '\n') {
        terminal_putchar(term, '\n');
        terminal_render(term);
        
        command_buffer[cmd_index] = '\0';
        shell_execute(term, command_buffer);
        cmd_index = 0;
        
        shell_prompt(term);
    } else if (c == '\b') {
        if (cmd_index > 0) {
            cmd_index--;
            terminal_putchar(term, '\b');
            terminal_render(term);
        }
    } else if (cmd_index < MAX_COMMAND_LENGTH - 1) {
        command_buffer[cmd_index++] = c;
        terminal_putchar(term, c);
        terminal_render(term);
    }
}

// Main shell loop
void shell_run_graphical(terminal_t* term) {
    shell_init_graphical(term);
    
    while (1) {
        if (keyboard_available()) {
            char c = keyboard_getchar();
            shell_handle_key_graphical(term, c);
        }
        __asm__ __volatile__("hlt");
    }
}
