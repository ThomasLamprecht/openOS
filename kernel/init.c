#include "console.h"
#include "intr.h"
#include "mm.h"
#include "multiboot.h"


void init(struct multiboot_info *multiboot_nfo)
{
	clrscr();
	kprintf("Weclome to codename 'Dragon Fruit'!\n\nFirst timer interrupt will initiate the scheduler...\n");

	/*
     * Nachdem die physische Speicherverwaltung initialisiert ist, duerfen wir
     * die Multiboot-Infostruktur nicht mehr benutzen, weil sie bei der
     * Initialisierung nicht reserviert wurde und daher nach dem ersten
     * pmm_alloc() ueberschrieben werden koennte.
     */
    pmm_init(multiboot_nfo);
    //mb_info = NULL;

	init_gdt();
	init_intr();
	init_multitasking(multiboot_nfo);

	asm volatile("int $0x20");
}
