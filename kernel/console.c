#include <stdarg.h>
#include <stdint.h>
#include "console.h"

static int x = 0, y = OSBAR; // actual x and y cursor position
static int kprintf_res = 0;
static uint8_t default_fg = COLOR_LIGHTGREEN, default_bg = COLOR_BLACK;
static char* video = (char*) 0xb8000; // start of the console memory

void kputccolor(char c, uint8_t color) // When porting on a tty this and the defines must be adapted
{
	if ((c == '\n') || (x >= XSIZE))
	{
		x = 0;
		y++;
		if(!(x >= XSIZE) || (c == '\n'))
		{
			return;
		}
	}
	if (y >= YSIZE)
	{
		int i;
		for (i = 0; i < 2 * (YSIZE-1) * XSIZE; i++)  // copy every line one up
		{
			video[i] = video[i + 160]; // TODO 2 * XSIZE ?!
		}
		for (; i < 2 * YSIZE * XSIZE; i++)
		{
			video[i] = 0;
		}
		y--;
	}
	
	kputchar(c, x,y, (color & 0x0F),(default_bg & 0xF0));

	x++;
	kprintf_res++;
}

static void kputc(char c)
{
	kputccolor(c, (uint8_t)((default_bg & 0xF0) | (default_fg & 0x0F))); // Using standart color...
}

void kputchar(char c, uint8_t x, uint8_t y, uint8_t fg, uint8_t bg)
{
	video[2 * (y * XSIZE + x)] = c;
	video[2 * (y * XSIZE + x) + 1] =  (bg & 0xF0) | (fg & 0x0F);
}

static void kputs(const char* s)
{
	while (*s)
	{
		kputc(*s++);
	}
}

static void kputn(unsigned long x, int base) // Calculates numbers in different base models
{
	char buf[65];
	const char* digits = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* p;

	if (base > 36)
	{
		return;
	}

	p = buf + 64;
	*p = '\0';
	do
	{
		*--p = digits[x % base];
		x /= base;
	} while (x);
	kputs(p);
}

void clrscr(void)
{
    int i;
    for (i = 0; i < 2 * YSIZE * XSIZE; i++)
    {
        video[i] = 0;
    }

    x = 0;
    y = OSBAR;
}

int kprintf(const char* fmt, ...)
{
	va_list ap;
	const char* s;
	unsigned long n;

	va_start(ap, fmt);
	kprintf_res = 0;
	while (*fmt)
	{
		if (*fmt == '%')
		{
			fmt++;
			switch (*fmt)
			{
				case 's':
				{
					s = va_arg(ap, char*);
					kputs(s);
					break;
				}
				case 'd': case 'u':
				{
					n = va_arg(ap, unsigned long int);
					kputn(n, 10);
					break;
				}
				case 'x': case 'p':
				{
					n = va_arg(ap, unsigned long int);
					kputn(n, 16);
					break;
				}
				case 'c':
				{
					n = va_arg(ap, int);
					kputc(n);
					break;
				}
				case '%':
				{
					kputc('%');
					break;
				}
				case '\0':
				{
					goto out;
				}
				default:  // right now there's missing a lot of formating stuff, but the important things to hel debugging is there, so who care :)
				{
					kputc('%');
					kputc(*fmt);
					break;
				}
			}
		}
		else
		{
			if(*fmt=='\t') // TODO find adaequat solution... (maybe looking how linus did this in 0.01)
			{
				int i;
				for(i=x%TABSIZE;i<TABSIZE;i++)
					kputc('\0');
			}
			else
			kputc(*fmt);
		}

		fmt++;
	}

out:
	va_end(ap);

	return kprintf_res;
}
