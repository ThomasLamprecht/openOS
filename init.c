#include "console.h"
#include "intr.h"

void init(void)
{
	clrscr();
	kprintf("Weclome to codename 'Banana'!\n\nFirst timer interrupt will initiate the scheduler...\n");
	
	init_gdt();
	init_intr();
	init_multitasking();
	
	//asm volatile("int $0x20");
}
