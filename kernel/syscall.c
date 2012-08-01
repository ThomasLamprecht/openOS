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
			kprintf("%s",cpu->ebx);
			break;
		}
		case SYSCALL_YIELD:
		{
			asm volatile("int $0x20");
			break;
		}
		case SYSCALL_EXIT:
		{
			
			if(deleteTask(sys_getPid())==TASK_DELETED)
				kprintf("__EXIT__ (%d)-> %d",sys_getPid(),cpu->ebx);
			break;
		}
		case SYSCALL_GETPID:
		{
			cpu->eax = sys_getPid();
			break;
		}
		default:
			kprintf("Unknown Syscall! %d\n",cpu->eax);
			break;
	}

	return cpu;
}
