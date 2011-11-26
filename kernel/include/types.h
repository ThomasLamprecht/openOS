#ifndef TYPES_H
#define TYPES_H

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

#endif