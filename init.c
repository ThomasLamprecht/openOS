#include "console.h"
#include "intr.h"

void init(void)
{
	clrscr();
	kprintf("Hello World!\nNow should be all ready :D\nTest: %d\n",22);
	kprintf("\tTabbed?\n");
	
	init_gdt();
	init_intr();
	
	asm volatile("int $0x20");
}
