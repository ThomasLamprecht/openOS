
#include "syscall.h"
#include <stdint.h>

uint16_t* videomem = (uint16_t*) 0xb8000;

void _start(void)
{
	//int i;
	char test[100] = "Hallo an so one\nGreetings from User space :D\n\0";
	//for (i = 0; i < 20; i++)
	//{
	// asm("xor eax,eax"::"b"('0'+i));
	// syscall(SYSCALL_PUTC); F*CK! -.- "searching solution" is on the roadmap :)
	
	asm ("int $0x30"::"a"(SYSCALL_PRINTF), "b"(test) );
	getPID();
	exit(0);
	
	while(1);
}
