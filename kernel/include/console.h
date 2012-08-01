#ifndef _CONSOLE_H
#define _CONSOLE_H

#define XSIZE 80
#define YSIZE 25
#define TABSIZE 4

#define OSBAR 0

#define COLOR_BLACK			0x00
#define COLOR_BLUE			0x01
#define COLOR_GREEN			0x02
#define COLOR_CYAN			0x03
#define COLOR_RED				0x04
#define COLOR_MAGENTA			0x05
#define COLOR_BROWN			0x06
#define COLOR_LIGHTGRAY		0x07
#define COLOR_DARKGRAY		0x08
#define COLOR_LIGHTBLUE		0x09
#define COLOR_LIGHTGREEN		0x0a
#define COLOR_LIGHTCYAN		0x0b
#define COLOR_LIGHTRED		0x0c
#define COLOR_LIGHTMAGENTA	0x0d
#define COLOR_YELLOW			0x0e
#define COLOR_WHITE			0x0f


static int x = 0, y = OSBAR; // actual x and y cursor position
static int kprintf_res = 0;
static uint8_t default_fg = COLOR_LIGHTGREEN, default_bg = COLOR_BLACK;
static char* video = (char*) 0xb8000; // start of the console memory

void clrscr(void);
int kprintf(const char* fmt, ...);
static void kputccolor(char c, uint8_t color);
static void kputchar(char c, uint8_t x, uint8_t y, uint8_t fg, uint8_t bg);
#endif
