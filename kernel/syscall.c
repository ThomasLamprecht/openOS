#include "syscall.h"

struct cpu_state* syscall(struct cpu_state* cpu)
{
	switch (cpu->eax)
	{
		case SYSCALL_PUTC: // putc
		{
			kprintf("%c", cpu->ebx);
			break;
		}
		case SYSCALL_PRINTF: // kprintf
		{
			kprintf("Test");
			break;
		}
		case SYSCALL_SCHEDULE:
		{
			asm volatile("int $0x20");
			break;
		}
	}

	return cpu;
}