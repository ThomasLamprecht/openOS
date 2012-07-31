#ifndef _CONSOLE_H
#define _CONSOLE_H

#define XSIZE 80
#define YSIZE 25
#define TABSIZE 4

#define OSBAR 0

void clrscr(void);
int kprintf(const char* fmt, ...);

#endif
