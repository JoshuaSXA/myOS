#ifndef MONITOR_H
#define MONITOR_H

#include "common.h"

// extern enum vga_color
// {
// 	COLOR_BLACK = 0,
// 	COLOR_BLUE = 1,
// 	COLOR_GREEN = 2,
// 	COLOR_CYAN = 3,
// 	COLOR_RED = 4,
// 	COLOR_MAGENTA = 5,
// 	COLOR_BROWN = 6,
// 	COLOR_LIGHT_GREY = 7,
// 	COLOR_DARK_GREY = 8,
// 	COLOR_LIGHT_BLUE = 9,
// 	COLOR_LIGHT_GREEN = 10,
// 	COLOR_LIGHT_CYAN = 11,
// 	COLOR_LIGHT_RED = 12,
// 	COLOR_LIGHT_MAGENTA = 13,
// 	COLOR_LIGHT_BROWN = 14,
// 	COLOR_WHITE = 15,
// };

// extern u8int make_color(enum vga_color fg, enum vga_color bg);

void monitor_put(char c);
void monitor_clear();
void monitor_write(char *s);
void monitor_write_hex(u32int n);
void monitor_write_dec(u32int n);
void print_welcome();
void boot_screen();
void trick();
void boot_message(char *s);


#endif
