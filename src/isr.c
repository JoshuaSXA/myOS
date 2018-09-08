#include "isr.h"
#include "monitor.h"

const char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

isr_t interrupt_handlers[256];
void register_interrupt_handler(u8int n, isr_t handler) {
	interrupt_handlers[n] = handler;
}

void isr_handler(registers_t regs) {
	if (interrupt_handlers[regs.int_no] != 0) {
		isr_t handler = interrupt_handlers[regs.int_no];
		handler(regs);
	} else {
		monitor_write("received CPU interrupt: ");
		monitor_write_dec(regs.int_no);
		monitor_put('\n');
	}
}

void irq_handler(registers_t regs) {
	if (regs.int_no > 40) {
		// slave reset
		outb(0xA0, 0x20);
	}

	// master reset
	outb(0x20, 0x20);
	
	if (interrupt_handlers[regs.int_no] != 0) {
		isr_t handler = interrupt_handlers[regs.int_no];
		handler(regs);
	} else {
		//monitor_write("received IRQ interrupt: ");
		//monitor_write_dec(regs.int_no);
		//monitor_put('\n');
	}
}

// void irq_handler(registers_t r)
// {
//    void (*handler)(registers_t r);

//    handler = interrupt_handlers[r.int_no];
//    if (handler)
//        handler(r);

//    if (r.int_no >= 40)
//    {
// 	outb(0xA0, 0x20);
//    }

//    outb(0x20, 0x20);
// }

void fault_handler(registers_t regs) {
    // move_cursor(make_color(COLOR_BLACK, COLOR_RED)); 
    if(regs.int_no < 32)
    {
        void (*handler)(registers_t regs);
        handler = interrupt_handlers[regs.int_no];
        if (handler)
            handler(regs);
    }
    monitor_write("\n%s\nException!!! System Halted!");

    for(;;);    
}
