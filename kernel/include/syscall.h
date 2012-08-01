#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"
#include "console.h"
#include "sys.h"

#define SYSCALL_PUTC 0
#define SYSCALL_PRINTF 1
#define SYSCALL_YIELD 2
#define SYSCALL_EXIT 3
#define SYSCALL_GETPID 4


struct cpu_state* syscall(struct cpu_state* cpu);

#endif
