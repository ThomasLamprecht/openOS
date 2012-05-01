#include "syscall.h"

void syscall(uint8_t call)
{
	asm volatile("int $0x30" : : "a" (call));
}

void exit(int8_t status)
{
	asm volatile("int $0x30" : : "a" (SYSCALL_EXIT), "b"(status));
	yield();
	while(1);
}
void yield()
{
	asm volatile("int $0x30" : : "a" (SYSCALL_YIELD));
}