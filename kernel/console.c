#include <stdarg.h>
#include "console.h"

static int x = 0; // actual x and y cursor position
static int y = 0;

static char* video = (char*) 0xb8000; // start of the console memory

static int kprintf_res = 0;

static void kputc(char c) // When porting on a other console this and the defines must be adapted
{
	if ((c == '\n') || (x >= XSIZE))
	{
		x = 0;
		y++;
	}

	if (c == '\n')
	{
		return;
	}

	if (y >= YSIZE)
	{
		int i;
		for (i = 0; i < 2 * (YSIZE-1) * XSIZE; i++)
		{
			video[i] = video[i + 160]; // TODO 2 * XSIZE ?!
		}

		for (; i < 2 * YSIZE * XSIZE; i++)
		{
			video[i] = 0;
		}
		y--;
	}

	video[2 * (y * XSIZE + x)] = c;
	video[2 * (y * XSIZE + x) + 1] = 0x07;

	x++;
	kprintf_res++;
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

    x = y = 0;
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
					s = va_arg(ap, char*);
					kputs(s);
					break;
				case 'd':
				case 'u':
					n = va_arg(ap, unsigned long int);
					kputn(n, 10);
					break;
				case 'x':
				case 'p':
					n = va_arg(ap, unsigned long int);
					kputn(n, 16);
					break;
				case '%':
					kputc('%');
					break;
				case '\0':
					goto out;
				default:
					kputc('%');
					kputc(*fmt);
					break;
			}
		}
		else
		{
			if(*fmt=='\t') // TODO find adaequat solution... (maybe looking how linus made this in 0.01)
			{
				int i;
				for(i=0;i<TABSIZE;i++)
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
