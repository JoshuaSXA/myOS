#include "common.h"

#define MULTIBOOT_FLAG_MEM		0x001
#define MULTIBOOT_FLAG_DEVICE	0x002
#define MULTIBOOT_FLAG_CMDLINE	0x004
#define MULTIBOOT_FLAG_MODS		0x008
#define MULTIBOOT_FLAG_AOUT		0x010
#define MULTIBOOT_FLAG_ELF		0x020
#define MULTIBOOT_FLAG_MMAP		0x040
#define MULTIBOOT_FLAG_CONFIG	0x080
#define MULTIBOOT_FLAG_LOADER	0x100
#define MULTIBOOT_FLAG_APM		0x200
#define MULTIBOOT_FLAG_VBE		0x400

struct multiboot {
	u32int flags;
	u32int mem_lower;
	u32int mem_upper;
	u32int boot_device;
	u32int cmdline;
	u32int mods_count;
	u32int *mods_addr;
	u32int num;
	u32int size;
	u32int addr;
} __attribute__((packed));
