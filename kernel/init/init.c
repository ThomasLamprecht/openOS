#include "console.h"
#include "intr.h"
#include "mm.h"
#include "multiboot.h"
#include "keyboard.h"


void init(struct multiboot_info *multiboot_nfo)
{
	clrscr();
	kprintf("Weclome to codename 'Dragon Fruit'!\n\nFirst timer interrupt will initiate the scheduler...\n");


    pmm_init(multiboot_nfo); // Pagin comming soon :)

	init_gdt();
	init_intr();

	init_keyboard();

	init_multitasking(multiboot_nfo);

	asm volatile("int $0x20"); // let's schedule
}
