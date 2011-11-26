#include <stdint.h>
#include "syscall.h"


void _start(void)
{
	int i;
	for (i = 0; i < 5; i++)
	{
		asm("xor eax,eax"::"b"('0'+i));
		syscall(SYSCALL_PUTC);
	}

	while(1);
}