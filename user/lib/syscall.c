#include "syscall.h"

void syscall(uint8_t call)
{
	asm volatile("int $0x30" : : "a" (call));
	return;
}

void exit(int8_t status)
{
	asm volatile("int $0x30" : : "a" (SYSCALL_EXIT), "b"(status));
	yield();
	while(1);
	return;
}
void yield()
{
	asm volatile("int $0x30" : : "a" (SYSCALL_YIELD));
	return;
}
uint32_t getPID()
{
	uint32_t pid=0;
	asm volatile("int $0x30" : "=a" (pid) : "a" (SYSCALL_GETPID));
	return pid;
}
