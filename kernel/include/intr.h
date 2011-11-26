#ifndef INTR_H
#define INTR_H

#include <stdint.h>

struct cpu_state
{
    // Von Hand gesicherte Register
    uint32_t   eax;
    uint32_t   ebx;
    uint32_t   ecx;
    uint32_t   edx;
    uint32_t   esi;
    uint32_t   edi;
    uint32_t   ebp;

    uint32_t   intr;
    uint32_t   error;

    // Von der CPU gesichert
    uint32_t   eip;
    uint32_t   cs;
    uint32_t   eflags;
    uint32_t   esp;
    uint32_t   ss;
};

void init_gdt(void);
void init_intr(void);
void init_multitasking(void);

uint8_t cmos_read(uint8_t offset);
void cmos_write(uint8_t offset,uint8_t val);

struct cpu_state* handle_interrupt(struct cpu_state* cpu);
struct cpu_state* schedule(struct cpu_state* cpu);

#endif
