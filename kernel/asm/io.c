#include "asm/io.h"

inline void outb(uint16_t port, uint8_t data)
{
	asm volatile ("outb %0, %1" : : "a" (data), "Nd" (port));
}

inline uint8_t inb(uint16_t port)
{
	uint8_t result=0;
	asm volatile ("inb %1, %0" : "=a" (result): "Nd" (port));
	return result;
}

inline void outb_p(uint16_t port, uint8_t data)
{
	__asm__ volatile ("outb %%al,%%dx\n"
	"\tjmp 1f\n"
	"1:\tjmp 1f\n"
	"1:"::"a" (data),"d" (port));
}
inline uint8_t inb_p(uint16_t port);