#ifndef INTR_H
#define INTR_H

#include <stdint.h>
#include "multiboot.h"
#include "types.h"
#include "syscall.h"

void init_gdt(void);
void init_intr(void);
void init_multitasking(struct multiboot_info* multiboot_nfo);

uint8_t cmos_read(uint8_t offset);
void cmos_write(uint8_t offset,uint8_t val);

struct cpu_state* handle_interrupt(struct cpu_state* cpu);
struct cpu_state* schedule(struct cpu_state* cpu);

#endif
