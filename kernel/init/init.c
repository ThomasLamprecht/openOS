#include "console.h"
#include "intr.h"
#include "mm.h"
#include "multiboot.h"
#include "keyboard.h"


void init(struct multiboot_info *multiboot_nfo)
{
	clrscr();
	kprintf("Welcome to our FREE (free as in free beer) OS! Actual codename 'Feijoa'!\n"
	"We <3 Opensource and naturally you too ;) Visit http://os.gamer-source.org\nSincerly, Tom <3\n");


    pmm_init(multiboot_nfo); // Paging comming soon :)

	init_gdt();
	init_intr();

	init_keyboard();

	init_multitasking(multiboot_nfo);
	outb(0x70,0x0b);
	outb(0x71,inb(0x71) | (0x03 << 1));
	//kprintf("%d\n",inb(0x71)&(0x01<<2));
//	outb(0x70,0x0b);
//	kprintf("%d\n",inb(0x71));

	asm volatile("int $0x20"); // lets schedule
}
