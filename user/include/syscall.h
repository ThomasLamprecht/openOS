#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#define SYSCALL_PUTC 0
#define SYSCALL_PRINTF 1
#define SYSCALL_YIELD 2
#define SYSCALL_EXIT 3

void syscall(uint8_t call);

void exit(int8_t status);
void yield();
#endif