#include "../../Lib/include/screen.h"
#include "../../Lib/include/keyboard.h"

#define MAX_COMMAND_LENGTH 256
#define MAX_HISTORY 10

// Command buffer
static char command_buffer[MAX_COMMAND_LENGTH];
static int cmd_index = 0;

// Command history
static char command_history[MAX_HISTORY][MAX_COMMAND_LENGTH];
static int history_count = 0;
static int history_index = 0;

// Uptime counter (incremented by timer if available)
static unsigned int uptime_seconds = 0;

// String compare
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// String length
int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

// String copy
void strcpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

// String starts with
int starts_with(const char* str, const char* prefix) {
    while (*prefix) {
        if (*str != *prefix) return 0;
        str++;
        prefix++;
    }
    return 1;
}

// Integer to string
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
    
    // Reverse string
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

// Hex to string
void itohex(unsigned int num, char* str) {
    const char hex_chars[] = "0123456789ABCDEF";
    int i = 0;
    
    str[i++] = '0';
    str[i++] = 'x';
    
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    
    int shift = 28;
    int started = 0;
    
    while (shift >= 0) {
        int digit = (num >> shift) & 0xF;
        if (digit != 0 || started) {
            str[i++] = hex_chars[digit];
            started = 1;
        }
        shift -= 4;
    }
    
    str[i] = '\0';
}

// Add to history
void add_to_history(const char* cmd) {
    if (strlen(cmd) == 0) return;
    
    // Don't add duplicate of last command
    if (history_count > 0 && strcmp(command_history[history_count - 1], cmd) == 0) {
        return;
    }
    
    if (history_count < MAX_HISTORY) {
        strcpy(command_history[history_count], cmd);
        history_count++;
    } else {
        // Shift history
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            strcpy(command_history[i], command_history[i + 1]);
        }
        strcpy(command_history[MAX_HISTORY - 1], cmd);
    }
    history_index = history_count;
}

// Print prompt
void shell_prompt() {
    screen_set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
    screen_print("root@seppuku");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_print(":");
    screen_set_color(COLOR_LIGHT_BLUE, COLOR_BLACK);
    screen_print("~");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_print("$ ");
}

// Execute command
void shell_execute(const char* cmd) {
    // Skip empty commands
    if (cmd[0] == '\0') {
        return;
    }
    
    add_to_history(cmd);
    
    // HELP command
    if (strcmp(cmd, "help") == 0) {
        screen_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
        screen_println("Available commands:");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        screen_println("  help      - Show this help message");
        screen_println("  clear     - Clear the screen");
        screen_println("  echo      - Echo text back");
        screen_println("  about     - About SEPPUKU OS");
        screen_println("  sysinfo   - Display system information");
        screen_println("  uptime    - Show system uptime");
        screen_println("  history   - Show command history");
        screen_println("  banner    - Display OS banner");
        screen_println("  color     - Test color output");
        screen_println("  meminfo   - Display memory information");
        screen_println("  cpuinfo   - Display CPU information");
        screen_println("  calc      - Simple calculator (e.g., calc 5+3)");
        screen_println("  date      - Show current date/time (simulated)");
        screen_println("  whoami    - Display current user");
        screen_println("  hostname  - Display system hostname");
        screen_println("  uname     - Print system information");
        screen_println("  reboot    - Reboot the system");
        return;
    }
    
    // CLEAR command
    if (strcmp(cmd, "clear") == 0) {
        screen_clear();
        return;
    }
    
    // ABOUT command
    if (strcmp(cmd, "about") == 0) {
        screen_set_color(COLOR_LIGHT_RED, COLOR_BLACK);
        screen_println("SEPPUKU OS v1.2");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        screen_println("A minimalist x86 operating system");
        screen_println("Built from scratch in C and Assembly");
        screen_println("");
        screen_println("Features:");
        screen_println("  - Protected mode");
        screen_println("  - IDT and interrupts");
        screen_println("  - Keyboard driver");
        screen_println("  - VGA text mode");
        screen_println("  - Enhanced shell with 15+ commands");
        screen_println("");
        screen_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
        screen_println("Created with dedication and passion");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        return;
    }
    
    // SYSINFO command
    if (strcmp(cmd, "sysinfo") == 0) {
        screen_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
        screen_println("=== System Information ===");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        screen_print("OS Name:        "); screen_println("SEPPUKU OS");
        screen_print("Version:        "); screen_println("1.2");
        screen_print("Architecture:   "); screen_println("x86 (i386)");
        screen_print("Boot Mode:      "); screen_println("Protected Mode (32-bit)");
        screen_print("Display:        "); screen_println("VGA Text Mode 80x25");
        screen_print("Kernel:         "); screen_println("Monolithic");
        return;
    }
    
    // UPTIME command
    if (strcmp(cmd, "uptime") == 0) {
        screen_set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
        screen_print("System uptime: ");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        
        char time_str[32];
        itoa(uptime_seconds, time_str);
        screen_print(time_str);
        screen_println(" seconds");
        
        // Simulate uptime increment
        uptime_seconds += 10;
        return;
    }
    
    // HISTORY command
    if (strcmp(cmd, "history") == 0) {
        if (history_count == 0) {
            screen_println("No command history available.");
        } else {
            screen_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
            screen_println("Command History:");
            screen_set_color(COLOR_WHITE, COLOR_BLACK);
            for (int i = 0; i < history_count; i++) {
                char num_str[8];
                itoa(i + 1, num_str);
                screen_print("  ");
                screen_print(num_str);
                screen_print(". ");
                screen_println(command_history[i]);
            }
        }
        return;
    }
    
    // BANNER command
    if (strcmp(cmd, "banner") == 0) {
        screen_set_color(COLOR_LIGHT_RED, COLOR_BLACK);
        screen_println("   _____ ______ _____  _____  _    _ _  ___    _    ___  _____ ");
        screen_println("  / ____|  ____|  __ \\|  __ \\| |  | | |/ / |  | |  / _ \\/ ____|");
        screen_println(" | (___ | |__  | |__) | |__) | |  | | ' /| |  | | | | | | (___  ");
        screen_println("  \\___ \\|  __| |  ___/|  ___/| |  | |  < | |  | | | | | |\\___ \\ ");
        screen_println("  ____) | |____| |    | |    | |__| | . \\| |__| | | |_| |____) |");
        screen_println(" |_____/|______|_|    |_|     \\____/|_|\\_\\\\____/   \\___/|_____/ ");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        screen_println("");
        screen_set_color(COLOR_YELLOW, COLOR_BLACK);
        screen_println("                    ~ Honor in Every Line ~");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        return;
    }
    
    // COLOR command
    if (strcmp(cmd, "color") == 0) {
        screen_println("Color test:");
        screen_set_color(COLOR_BLACK, COLOR_WHITE);
        screen_print(" BLACK ");
        screen_set_color(COLOR_BLUE, COLOR_BLACK);
        screen_print(" BLUE ");
        screen_set_color(COLOR_GREEN, COLOR_BLACK);
        screen_print(" GREEN ");
        screen_set_color(COLOR_CYAN, COLOR_BLACK);
        screen_print(" CYAN ");
        screen_set_color(COLOR_RED, COLOR_BLACK);
        screen_print(" RED ");
        screen_set_color(COLOR_MAGENTA, COLOR_BLACK);
        screen_print(" MAGENTA ");
        screen_println("");
        screen_set_color(COLOR_BROWN, COLOR_BLACK);
        screen_print(" BROWN ");
        screen_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
        screen_print(" LIGHT_GREY ");
        screen_set_color(COLOR_DARK_GREY, COLOR_BLACK);
        screen_print(" DARK_GREY ");
        screen_set_color(COLOR_LIGHT_BLUE, COLOR_BLACK);
        screen_print(" LIGHT_BLUE ");
        screen_println("");
        screen_set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
        screen_print(" LIGHT_GREEN ");
        screen_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
        screen_print(" LIGHT_CYAN ");
        screen_set_color(COLOR_LIGHT_RED, COLOR_BLACK);
        screen_print(" LIGHT_RED ");
        screen_println("");
        screen_set_color(COLOR_LIGHT_MAGENTA, COLOR_BLACK);
        screen_print(" LIGHT_MAGENTA ");
        screen_set_color(COLOR_YELLOW, COLOR_BLACK);
        screen_print(" YELLOW ");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        screen_println(" WHITE ");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        return;
    }
    
    // MEMINFO command
    if (strcmp(cmd, "meminfo") == 0) {
        screen_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
        screen_println("=== Memory Information ===");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        screen_println("Total RAM:      32 MB");
        screen_println("Kernel Memory:  0x1000 - 0x9000");
        screen_println("Stack:          0x90000");
        screen_println("VGA Memory:     0xB8000");
        screen_set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
        screen_println("Memory management: Basic (no paging yet)");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        return;
    }
    
    // CPUINFO command
    if (strcmp(cmd, "cpuinfo") == 0) {
        screen_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
        screen_println("=== CPU Information ===");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        screen_println("Architecture:   x86 (i386)");
        screen_println("Mode:           32-bit Protected Mode");
        screen_println("Features:       IDT, IRQ handling");
        screen_println("Interrupts:     Enabled");
        return;
    }
    
    // CALC command (simple calculator)
    if (starts_with(cmd, "calc ")) {
        const char* expr = cmd + 5;
        int num1 = 0, num2 = 0;
        char op = 0;
        int i = 0;
        
        // Parse first number
        while (expr[i] >= '0' && expr[i] <= '9') {
            num1 = num1 * 10 + (expr[i] - '0');
            i++;
        }
        
        // Get operator
        op = expr[i];
        i++;
        
        // Parse second number
        while (expr[i] >= '0' && expr[i] <= '9') {
            num2 = num2 * 10 + (expr[i] - '0');
            i++;
        }
        
        int result = 0;
        int valid = 1;
        
        if (op == '+') result = num1 + num2;
        else if (op == '-') result = num1 - num2;
        else if (op == '*') result = num1 * num2;
        else if (op == '/') {
            if (num2 != 0) result = num1 / num2;
            else {
                screen_set_color(COLOR_LIGHT_RED, COLOR_BLACK);
                screen_println("Error: Division by zero!");
                screen_set_color(COLOR_WHITE, COLOR_BLACK);
                valid = 0;
            }
        }
        else {
            screen_set_color(COLOR_LIGHT_RED, COLOR_BLACK);
            screen_println("Error: Invalid operator! Use +, -, *, /");
            screen_set_color(COLOR_WHITE, COLOR_BLACK);
            valid = 0;
        }
        
        if (valid) {
            char result_str[32];
            itoa(result, result_str);
            screen_set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
            screen_print("Result: ");
            screen_set_color(COLOR_WHITE, COLOR_BLACK);
            screen_println(result_str);
        }
        return;
    }
    
    // DATE command
    if (strcmp(cmd, "date") == 0) {
        screen_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
        screen_println("Current date/time (simulated):");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        screen_println("Mon Jan 1 00:00:00 UTC 2024");
        screen_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
        screen_println("Note: Real-time clock not yet implemented");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        return;
    }
    
    // WHOAMI command
    if (strcmp(cmd, "whoami") == 0) {
        screen_set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
        screen_println("root");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        return;
    }
    
    // HOSTNAME command
    if (strcmp(cmd, "hostname") == 0) {
        screen_set_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
        screen_println("seppuku");
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        return;
    }
    
    // UNAME command
    if (strcmp(cmd, "uname") == 0 || strcmp(cmd, "uname -a") == 0) {
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        if (strcmp(cmd, "uname -a") == 0) {
            screen_println("SEPPUKU 1.2 seppuku i386 x86");
        } else {
            screen_println("SEPPUKU");
        }
        return;
    }
    
    // ECHO command
    if (starts_with(cmd, "echo ")) {
        screen_set_color(COLOR_YELLOW, COLOR_BLACK);
        screen_println(cmd + 5);
        screen_set_color(COLOR_WHITE, COLOR_BLACK);
        return;
    }
    
    // REBOOT command
    if (strcmp(cmd, "reboot") == 0) {
        screen_set_color(COLOR_YELLOW, COLOR_BLACK);
        screen_println("Rebooting system...");
        screen_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
        screen_println("Goodbye!");
        
        // Small delay
        for (volatile int i = 0; i < 10000000; i++);
        
        // Reboot via keyboard controller
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
    screen_set_color(COLOR_LIGHT_RED, COLOR_BLACK);
    screen_print("seppuku: command not found: ");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_println(cmd);
    screen_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    screen_println("Type 'help' for available commands.");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
}

// Initialize shell
void shell_init() {
    cmd_index = 0;
    history_count = 0;
    history_index = 0;
    uptime_seconds = 0;
    
    screen_set_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    screen_println("");
    screen_println("Welcome to SEPPUKU OS Shell!");
    screen_set_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    screen_println("Type 'help' for available commands.");
    screen_set_color(COLOR_WHITE, COLOR_BLACK);
    screen_println("");
    shell_prompt();
}

// Handle keyboard input
void shell_handle_key(char c) {
    if (c == '\n') {
        // Execute command
        screen_putchar('\n');
        command_buffer[cmd_index] = '\0';
        shell_execute(command_buffer);
        cmd_index = 0;
        shell_prompt();
    } else if (c == '\b') {
        // Backspace
        if (cmd_index > 0) {
            cmd_index--;
            screen_putchar('\b');
        }
    } else if (cmd_index < MAX_COMMAND_LENGTH - 1) {
        // Normal character
        command_buffer[cmd_index++] = c;
        screen_putchar(c);
    }
}

// Main shell loop
void shell_run() {
    shell_init();
    
    while (1) {
        if (keyboard_available()) {
            char c = keyboard_getchar();
            shell_handle_key(c);
        }
        __asm__ __volatile__("hlt");
    }
}
