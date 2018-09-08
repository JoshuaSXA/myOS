#include "multiboot.h"
#include "monitor.h"
#include "gdt-idt.h"
#include "timer.h"
#include "kheap.h"
#include "paging.h"
#include "fs.h"
#include "task.h"
#include "syscall.h"
#include "keyboard.h"
#include "shell.h"

extern u32int end;
extern u32int placement_address;
extern heap_t *kheap;
extern fs_node_t *initrd_root;
extern fs_node_t *initrd_dev;
extern fs_node_t *root_nodes;
extern isr_t interrupt_handlers[];

u32int initial_esp;

int main(struct multiboot *mboot_ptr, u32int initial_stack) {
	initial_esp = initial_stack;
	boot_screen();

	init_gdt();

	init_idt();
	memset(&interrupt_handlers, 0, sizeof(isr_t)*256);

	asm volatile("sti");
	init_timer(50); // 50Hz

	ASSERT(mboot_ptr->mods_count > 0);
	u32int initrd_location = mboot_ptr->mods_addr[0];
	u32int initrd_end = mboot_ptr->mods_addr[1];
	placement_address = initrd_end;

	init_paging();

	init_tasking();

	init_initrd(initrd_location);

	init_syscalls();

	// switch_to_user_mode();

	keyboard_install();

	duplicate_shell();

	while(1);
	return 0;
}
