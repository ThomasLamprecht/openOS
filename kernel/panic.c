#include "panic.h"

void panic(struct cpu_state* cpu)
{
	const char *emesg[] =
	{
		"'Divide by zero' occurred",
		"'Single step' occurred, debugging pls",
		"'Non maskable' occurred",
		"'Breakpoint Exception' occurred",
		"'Overflow Exception' occured",
		"BOUND range exceeded",
		"Invalid opcode",
		"Coprocessor not available", // we should emulate it, but I don't think that thus exception occurs nowadays
		"'Double fault' occurred",
		"Coprocessor segment overrun",
		"Invalid task state segment",
		"Segment not present",
		"Stack exception",
		"General protection exception",
		"Page fault", // Shouldn't occur 'til we've implemented paging ;)
		"Reserved o.O",
		"Coprocessor error"
	};
	kprintf("\nE %d, Panic: \"%s\" errno: %x\n", cpu->intr, emesg[cpu->intr],cpu->error);
	kprintf("eax: 0x%x;\t"
	"ebx: 0x%x;\n"
	"ecx: 0x%x;\t"
	"edx: 0x%x;\n"
	"esi: 0x%x;\t"
	"edi: 0x%x;\n"
	"ebp: 0x%x;\t"
	"eip: 0x%x;\n"
	"cs : 0x%x;\t"
	"eflags: 0x%x;\n"
	"esp: 0x%x;\t"
	"ss : 0x%x\n",cpu->eax,cpu->ebx,cpu->ecx,cpu->edx,cpu->esi,cpu->edi,cpu->ebp,cpu->eip,cpu->cs,cpu->eflags,cpu->esp,cpu->ss);

	while(1)
	{
		asm volatile("cli; hlt"); // stopping cpu, we should kill the guilty task and go one
	}
}