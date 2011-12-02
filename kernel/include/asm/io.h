#ifndef _IO_H
#define _IO_H

#include <stdint.h>

/*#define outb(value,port) \
asm volatile ("outb %%al, %%dx"::"a" (value),"d" (port))


#define inb(port) ({ \
unsigned char _v; \
__asm__ volatile ("inb %%dx,%%al":"=a" (_v):"d" (port)); \
_v; \
})

#define outb_p(value,port) \
__asm__ volatile ("outb %%al,%%dx\n" \
		"\tjmp 1f\n" \
		"1:\tjmp 1f\n" \
		"1:"::"a" (value),"d" (port))

#define inb_p(port) ({ \
unsigned char _v; \
__asm__ volatile ("inb %%dx,%%al\n" \
	"\tjmp 1f\n" \
	"1:\tjmp 1f\n" \
	"1:":"=a" (_v):"d" (port)); \
_v; \
})*/
inline void outb(uint16_t port, uint8_t data);
inline uint8_t inb(uint16_t port);
inline void outb_p(uint16_t port, uint8_t data);
inline uint8_t inb_p(uint16_t port);

#endif