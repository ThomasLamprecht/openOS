#include <stdint.h>
//#include "syscall.h"

uint16_t* videomem = (uint16_t*) 0xb8000;

void _start(void)
{
	int i;
    for (i = 0; i < 10; i++) {
        *videomem++ = (0x06 << 8) | ('0' + i);
    }

    while(1);
}