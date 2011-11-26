#include "syscall.h"

void syscall(uint8_t call)
{
	asm volatile("int $0x30" : : "a" (call));
}