#ifndef _PANIC_H
#define _PANIC_H

#include <stdint.h>
#include "console.h"
#include "types.h"

void panic(struct cpu_state* cpu);
#endif