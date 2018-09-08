#include "common.h"

static u16int *video_memory = (u16int *)0xB8000;

static u8int x = 0;
static u8int y = 0;

// bg: black fg: white
static u16int	attr  = 0x0f00;
static u16int	blank = 0x0f20;

// u8int make_color(enum vga_color fg, enum vga_color bg)
// {
// 	return fg | bg << 4;
// }

void print_welcome() {
	monitor_write("_____________________________________________________\n");
	monitor_write(".....................................................\n");
	monitor_write("....cccccccccc..............oooooooooo..sssssssss....\n");
	monitor_write("....ccccccccc...............oooooooooo..sssssssss....\n");
	monitor_write("....ccc.............cccccc..o.......oo..ss...........\n");
	monitor_write("....ccc.......ooooooccccc...o.......oo..ss...........\n");
	monitor_write("....ccc.......ooooooc.......o.......oo..sssssssss....\n");
	monitor_write("....ccc.......o....oc.......o.......oo........sss....\n");
	monitor_write("....cccccccccco....occccccc.oooooooooosssssssssss....\n");
	monitor_write("....cccccccccccoooooccccccccooooooooossssssssssss....\n");
	monitor_write(".....................................................\n");
	monitor_write("_____________________________________________________\n");
	monitor_write("Coc Operating System version 0.2.1\n");
	monitor_write("Author Dcalsky & nemoremold\n");
	monitor_write("_____________________________________________________\n");
}

void boot_screen() {
	print_welcome();
	monitor_write("Booting from floppy: ");
}

void boot_message(char *s) {
	monitor_write(s);
	trick();
}

void trick() {
    for (int i = 0; i < 12000000; ++i);
}

static void move_cursor() {
	u16int pos = y * 80 + x;
	outb(0x3D4, 14); // cursor pos high
	outb(0x3D5, pos >> 8);
	outb(0x3D4, 15); // cursor pos low
	outb(0x3D5, pos);
}

static void scroll() {
	if (y < 25) {
		return;
	}
	int i;
	for (i = 0 * 80; i < 24 * 80; i++) {
		video_memory[i] = video_memory[i+80];
	}

	for (i = 24 * 80; i < 25 * 80; i++) {
		video_memory[i] = blank;
	}

	y = 24;
}

void monitor_put(char c) {
	u16int *pos;

	if (c == 0x08 && x) { // backspace
		pos = video_memory + y * 80 + x - 1;		
		*pos = blank;
		x--;
	} else if (c == '\t') {
		if (x % 4) {
			x += 4 - x % 4;
		} else {
			x += 4;
		}
	} else if (c == '\r') {
		x = 0;
	} else if (c == '\n') {
		x = 0;
		y++;
	} else if (c >= ' ') {
		pos = video_memory + y * 80 + x;		
		*pos = attr | c;
		x++;
	}

	if (x >= 80) {
		x = 0;
		y++;
	}

	scroll();
	move_cursor();
}

void monitor_clear() {
	int i;
	for (i = 0; i< 80 * 25; i++) {
		video_memory[i] = blank;
	}

	x = 0;
	y = 0;
	move_cursor();
}

void monitor_write(char *s) {
	int i = 0;
	while (s[i]) {
		monitor_put(s[i]);
		i++;
	}
}

void monitor_write_hex(u32int n) {
	s32int tmp;

	monitor_write("0x");

	char noZeros = 1;

	int i;
	for (i = 28; i > 0; i -= 4) {
		tmp = (n >> i) & 0x0F;
		if (tmp == 0 && noZeros != 0) {
			continue;
		}
		if (tmp >= 0x0A) {
			noZeros = 0;
			monitor_put(tmp - 0x0A + 'a');
		} else {
			noZeros = 0;
			monitor_put(tmp + '0');
		}
	}

	tmp = n & 0x0F;
	if (tmp >= 0x0A) {
		monitor_put(tmp - 0x0A + 'a');
	} else {
		monitor_put(tmp + '0');
	}
}

void monitor_write_dec(u32int n) {
	if (n == 0) {
		monitor_put('0');
		return;
	}
	
	s32int acc = n;
	char c[32];
	int i = 0;
	while (acc > 0) {
		c[i] = '0' + acc % 10;
		acc /= 10;
		i++;
	}
	c[i] = 0;

	char c2[32];
	c2[i--] = 0;
	int j = 0;
	while (i >= 0) {
		c2[i--] = c[j++];
	}
	monitor_write(c2);
}
