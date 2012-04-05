#include <stdint.h>
#include "syscall.h"

uint16_t* videomem = (uint16_t*) 0xb8000;

void _start(void)
{
		int i;
	//	for (i = 0; i < 10; i++) {
	//		*videomem++ = (0x06 << 8) | ('0' + i);
	//	}
	for (i = 0; i < 20; i++)
	{
	// asm("xor eax,eax"::"b"('0'+i));
	// syscall(SYSCALL_PUTC); F*CK! -.- "searching solution" is on the roadmap :)
	asm ("int $0x30"::"a"(SYSCALL_PUTC), "b"('0'+i));

	}
	while(1);
}