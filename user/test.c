#include <stdint.h>
#include "syscall.h"


void _start(void)
{
	int i;
	for (i = 0; i < 5; i++)
	{
		// asm("xor eax,eax"::"b"('0'+i));
		// syscall(SYSCALL_PUTC); F*CK! -.- "searching solution" is on the roadmap :)
		asm ("int $0x30"::"a"(SYSCALL_PUTC), "b"('0'+i));
	}

	while(1);
}